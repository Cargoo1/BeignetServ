#include <DeleteMethod.hpp>

#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>

DeleteMethod::DeleteMethod(ExecutionContext &context) : HttpMethod(context) {};

DeleteMethod::~DeleteMethod() {};

void DeleteMethod::executeMethod(HttpResponse &rsp) {
	
	std::string path = this->_context.location.getRoot() + this->_context.request.getHeader().getTargetResource();
	// std::string path = NormalizePath(this->_context.location.getRoot()) + NormalizePath(this->_context.location.getPath());
	struct stat path_stat;
	if (stat(path.c_str(), &path_stat) < 0)
		throw Request::ErrorRequest(not_found);
	if (access(path.c_str(), W_OK) != 0)
		throw Request::ErrorRequest(forbiden);
	if (S_ISDIR(path_stat.st_mode)) {
		if (!_removeDirectoryRecursive(path))
			throw Request::ErrorRequest(internal_server_error);
	}
	else if (!std::remove(path.c_str()))
		throw Request::ErrorRequest(internal_server_error);
	
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