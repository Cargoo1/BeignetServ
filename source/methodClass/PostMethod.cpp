#include <PostMethod.hpp>

#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sys/stat.h>

namespace { std::size_t toSizeT(const std::string &str) {
	std::stringstream ss(str);
	std::size_t ret;
	ss >> ret;
	return (ret);
} }

namespace { bool isFile(const std::string &str) {
	std::string::size_type pos = str.find(".");
	if (pos == std::string::npos) {
		return (false);
	}
	std::string compare = str.substr(pos); 
	if (compare == ".txt" || compare == ".png" || compare == ".jpg" || compare == ".pdf")
		return (true);
	return (false);
} }

namespace { bool createFile(const std::string &fileName, const std::string &body) {
	std::ofstream ofs;
	struct stat path_stat;
	if (stat(fileName.c_str(), &path_stat) == 0) {
		std::cerr << fileName << " exist already" << std::endl;
		return (false);
	}
	ofs.open(fileName.c_str());
	if (!ofs.is_open()) {
		std::cerr << "creating " << fileName << " failed" << std::endl;
		return (false);
	}
	ofs.write(body.c_str(), body.size());
	ofs.close();
	return (true);
} }

namespace { bool postFile(std::string path, const std::string &body){
	std::istringstream iss(path);
	std::string part;
	struct stat path_stat;
	const std::size_t size = 1024; 
	char buffer[size];

	if (getcwd(buffer, size) == NULL) {
		std::cerr << "Error getting current working directory" << std::endl;
		return (false);
	}
	while (std::getline(iss, part, '/')) {
		if (part.empty())
			continue;
		if (isFile(part)) {
			if (!createFile(part, body))
				return (false);
			continue;
		}
		if (stat(part.c_str(), &path_stat) != 0) {
			if (mkdir(part.c_str(), S_IRWXU) != 0) {
				std::cerr << strerror(errno) << std::endl;
				return (false);
			}
		}
		if (chdir(part.c_str()) != 0) {
			std::cerr << strerror(errno) << std::endl;
			return (false);
		}
	}
	if (chdir(buffer) != 0) {
		std::cerr << strerror(errno) << std::endl;
		return (false);
	}
	return (true);
} }

PostMethod::PostMethod(ExecutionContext &context) : HttpMethod(context) {};

PostMethod::~PostMethod() {};

void PostMethod::executeMethod(HttpResponse &rsp) {
	std::string path;
	if (this->_context.location.getUploadStore().empty())
		path = this->_context.location.getRoot() + this->_context.request.getHeader().getTargetResource();
	else
		path = this->_context.location.getUploadStore() + this->_context.request.getHeader().getTargetResource();
	std::size_t contentLenght = toSizeT(this->_context.request.getHeader().getContentLenght());
	if (this->_context.location.hasCMBS()) {
		if (contentLenght > this->_context.location.getCMBS())
			throw Request::ErrorRequest(content_too_large);
	}
	if (!postFile(path, this->_context.request.getBody()))
		throw Request::ErrorRequest(internal_server_error);
	
}