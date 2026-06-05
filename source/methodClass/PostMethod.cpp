#include <PostMethod.hpp>
#include <UtilsMethod.hpp>

#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <ctime>
#include <sys/stat.h>

namespace { std::string generateTimestampFilename(std::string filename){
	std::string::size_type pos = filename.find(".");
	std::cout << filename << std::endl;
	if (pos == std::string::npos)
		return (filename);
	std::string tmp = filename.substr(0, pos);
	std::time_t resultTime = std::time(0);
	std::string timestamp = toStr(resultTime);
	std::string newFileName = tmp + "-" + timestamp;
	tmp = filename.substr(pos);
	newFileName += tmp;
	return (newFileName);
} }

namespace { bool createFile(const std::string &fileName, const std::string &body) {
	std::ofstream ofs;
	ofs.open(fileName.c_str(), std::ios::binary);
	if (!ofs.is_open()) {
		std::cerr << "creating " << fileName << " failed" << std::endl;
		return (false);
	}
	ofs.write(body.c_str(), body.size());
	ofs.close();
	return (true);
} }

namespace { bool postFile(std::string path, const std::string &body){
	std::size_t found = path.find_last_of("/\\");
	std::string newPath = path.substr(0, found);
	std::istringstream iss(newPath);
	std::string file = path.substr(found+1);
	std::string part, current_path = "";
	struct stat path_stat;

	if (found != std::string::npos) {
		while (std::getline(iss, part, '/')) {
			if (part.empty())
				continue;
			current_path += part;
			if (stat(current_path.c_str(), &path_stat) != 0) {
				if (mkdir(current_path.c_str(), S_IRWXU) != 0) {
					std::cerr << "Post method mkdir failed" << std::endl;
					return (false);
				}
			}
			current_path += "/";
		}
	}
	if (!file.empty()) {
		std::string tmp(current_path);
		current_path += file;
		int ret = stat(current_path.c_str(), &path_stat);
		if (ret == 0)
			file = generateTimestampFilename(file);
		tmp += file;
		current_path = tmp;
		if (!createFile(current_path, body)) {
			return (false);
		}
	}
	return (true);
} }

namespace { std::size_t toSizeT(const std::string &str) {
	std::stringstream ss(str);
	std::size_t ret;
	ss >> ret;
	return (ret);
} }

PostMethod::PostMethod(ExecutionContext &context) : HttpMethod(context) {};

PostMethod::~PostMethod() {};

void PostMethod::executeMethod(HttpResponse &rsp) {
	std::string targetResource = this->_context.request.getHeader().getTargetResource();

	std::string path;
	if (this->_context.location.getUploadStore().empty())
		path = this->_context.location.getRoot() + targetResource;
	else
		path = this->_context.location.getUploadStore() + targetResource;

	std::size_t contentLenght = toSizeT(this->_context.request.getHeader().getContentLenght());
	if (this->_context.location.hasCMBS()) {
		if (contentLenght > this->_context.location.getCMBS())
			throw Request::ErrorRequest(content_too_large);
	}

	std::string body = "";
	if (targetResource.back() != '/')
		body = this->_context.request.getBody();

	if (!postFile(path, body))
		throw Request::ErrorRequest(internal_server_error);

	rsp.setStatusCode(201);
}