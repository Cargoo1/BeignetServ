#include "Request.hpp"
#include <DeleteMethod.hpp>

#include <dirent.h>
#include <unistd.h>
#include <cstdio>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

DeleteMethod::DeleteMethod(Request const& r) : HttpMethod(r) {};

DeleteMethod::~DeleteMethod() {};

void DeleteMethod::executeMethod(HttpResponse &rsp) {
	std::string path = this->_request.getHeader().getTargetResource();
	struct stat path_stat;
	if (stat(path.c_str(), &path_stat) < 0) {
		std::string error_msg = strerror(errno);
		std::string msg = "DELETE: " + error_msg;
		throw Request::ErrorRequest(not_found, msg.c_str());
	}
	if (access(path.c_str(), W_OK) != 0)
		throw Request::ErrorRequest(forbiden, "Access forbiden");
	if (S_ISDIR(path_stat.st_mode)) {
		if (!_removeDirectoryRecursive(path))
			throw Request::ErrorRequest(internal_server_error, "DELETE: removal function failed");
	}
	else if (std::remove(path.c_str()) != 0) {
		std::string error_msg = strerror(errno);
		std::string msg = "DELETE: " + error_msg;
		throw Request::ErrorRequest(internal_server_error, msg.c_str());
	}
	rsp.setStatusCode(204);
}

bool DeleteMethod::_removeDirectoryRecursive(const std::string &dirPath){
	std::string tmp_filePath;
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir (dirPath.c_str())) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
				continue;
			else {
				tmp_filePath = dirPath + "/" + ent->d_name;
				if (ent->d_type == DT_DIR) {
					if (!_removeDirectoryRecursive(tmp_filePath))
						return (false);
				}
				else if (std::remove(tmp_filePath.c_str()) != 0) {
					return (false);
				}
			}
		}
		if (closedir (dir) != 0)
			return (false);
		if (rmdir(dirPath.c_str()) != 0)
			return (false);
	} 
	else {
		return (false);
	}
	return (true);
}
