#include "Request.hpp"
#include <PostMethod.hpp>
#include <utils.hpp>

#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <ctime>
#include <sys/stat.h>

namespace { std::string generateTimestampFilename(std::string filename) {
	std::time_t resultTime = std::time(0);
	std::string timestamp = toStr(resultTime);
	if (filename.empty()) {
		return (timestamp);
	}
	std::string tmp;
	std::string::size_type pos = filename.find(".");
	if (pos == std::string::npos)
		return (filename);
	tmp = filename.substr(0, pos);
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
	if (!body.empty()) {
		if (file.empty())
			file = generateTimestampFilename(file);
		std::string tmp(current_path);
		current_path += file;
		int ret = stat(current_path.c_str(), &path_stat);
		if (ret == 0)
			file = generateTimestampFilename(file);
		tmp += file;
		current_path = tmp;
			if (!createFile(current_path, body))
				return (false);
	}
	return (true);
} }

namespace { std::size_t toSizeT(const std::string &str) {
	std::stringstream ss(str);
	std::size_t ret;
	ss >> ret;
	return (ret);
} }

PostMethod::PostMethod(Request const& r) : HttpMethod(r) {};

PostMethod::~PostMethod() {};

void PostMethod::executeMethod(HttpResponse &rsp) {
	std::string path = this->_request.getHeader().getTargetResource();
	const locationConfig *loc = this->_request.getLocConfBlock();
	if (loc->getUploadStore().empty())
		throw Request::ErrorRequest(internal_server_error, "POST: no Upload store avaliable");
	std::size_t contentLenght = toSizeT(this->_request.getHeader().getContentLenght());
	if (this->_request.getLocConfBlock()->hasCMBS()) {
		if (contentLenght > this->_request.getLocConfBlock()->getCMBS())
			throw Request::ErrorRequest(content_too_large, "POST: the files is too heavy (> client max body size)");
	}
	std::string body = "";
	if (path.at(path.size()-1) != '/')
		body = this->_request.getBody();
	if (!postFile(path, body))
		throw Request::ErrorRequest(internal_server_error, "POST: postFile(path, body) failed");

	rsp.setStatusCode(201);
}
