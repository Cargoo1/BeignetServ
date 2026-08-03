#include "Request.hpp"
#include "utils_execution.hpp"
#include <PostMethod.hpp>
#include <cerrno>
#include <utils.hpp>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <ctime>
#include <sys/stat.h>

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

namespace { bool postFile(std::string &path, const std::string &body, std::string &filename){
	std::size_t found = path.find_last_of("/\\");
	std::string newPath = path.substr(0, found);
	std::istringstream iss(newPath);
	std::string part, current_path = "";
	struct stat path_stat;
	int stat_ret;

	if (found != std::string::npos) {
		while (std::getline(iss, part, '/')) {
			if (part.empty())
				continue;
			current_path += part;
			stat_ret = stat(current_path.c_str(), &path_stat);
			if (stat_ret == 0 && !S_ISDIR(path_stat.st_mode)) {
				std::cerr << "Post method: " << current_path << " exists and is not a directory" << std::endl;
				return (false);
			}
			if (stat_ret != 0 && mkdir(current_path.c_str(), S_IRWXU) != 0) {
				std::cerr << "Post method mkdir failed" << std::endl;
				return (false);
			}
			current_path += "/";
		}
	}
	std::string tmp(current_path);
	int ret = stat(current_path.c_str(), &path_stat);
	if (ret == 0) {
		generateTimestampFilename(filename);
		tmp += filename;
		current_path = tmp;
	}
	if (!createFile(current_path, body))
		return (false);
	return (true);
} }

/*
namespace { std::size_t toSizeT(const std::string &str) {
	std::stringstream ss(str);
	std::size_t ret;
	ss >> ret;
	return (ret);
} }
*/

PostMethod::PostMethod(Request const& r) : HttpMethod(r) {};

PostMethod::~PostMethod() {};

//CHANGEMENTS!! g pu recuperer le filename et je le mets dans le HEADER getFilename
//si filename c vide ca veut dire qu'on a pas le filename


void PostMethod::executeMethod(HttpResponse &rsp) {
	const locationConfig *loc = this->_request.getLocConfBlock();
	std::string path = this->_request.getHeader().getTargetResource();

	if (loc->getUploadStore().empty())
		throw Request::ErrorRequest(internal_server_error, "POST: no Upload store avaliable");

	std::size_t contentLenght = this->_request.getBodyLen();
	if (this->_request.getLocConfBlock()->hasCMBS()) {
		if (contentLenght > this->_request.getLocConfBlock()->getCMBS())
			throw Request::ErrorRequest(content_too_large, "POST: the files is too heavy (> client max body size)");
	}

	std::string filename;
	find_filename(this->_request.getHeader(), filename);

	std::string body = "";
	body = this->_request.getBody();
	if (!postFile(path, body, filename))
		throw Request::ErrorRequest(internal_server_error, "POST: postFile(path, body) failed");

	rsp.setStatusCode(204);
	rsp.addField("Content-Length", "0");
}
