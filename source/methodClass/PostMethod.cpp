#include <PostMethod.hpp>

#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>

namespace { std::size_t toSizeT(const std::string &str) {
	std::stringstream ss(str);
	std::size_t ret;
	ss >> ret;
	return (ret);
} }

namespace { bool isDir(const std::string &path){
	if (!path.empty()) {
		if (path.at(path.size() - 1) == '/')
			return (true);
	}
	return (false);
} }

namespace { std::vector<std::string> ft_split(std::string path) {
	std::vector<std::string> vectStr;
	std::string tmp;
	for (std::size_t i = 0; i < path.size(); i++) {
		if (path.at(i) != '/')
			tmp += path.at(i);
		else {
			if (tmp != "")
				vectStr.push_back(tmp);
			tmp.clear();
		}
	}
	if (tmp != "")
		vectStr.push_back(tmp);
	return (vectStr);
} }

namespace { bool createDirectoriesRecursive(std::string path){
	std::vector<std::string> vectStr = ft_split(path);
	std::string tmp = vectStr.pop_back();
	while ()
} }

PostMethod::PostMethod(ExecutionContext &context) : HttpMethod(context) {};

PostMethod::~PostMethod() {};

HttpResponse &PostMethod::executeMethod() {
	std::string path;
	HttpResponse ret(201);
	if (this->_context.location.getUploadStore().empty())
		path = NormalizePath(this->_context.location.getRoot()) + NormalizePath(this->_context.request.getHeader().getTargetResource());
	path = NormalizePath(this->_context.location.getUploadStore()) + NormalizePath(this->_context.request.getHeader().getTargetResource());
	std::size_t contentLenght = toSizeT(this->_context.request.getHeader().getContentLenght());
	if (this->_context.location.hasCMBS()) {
		if (contentLenght > this->_context.location.getCMBS()) {
			ret.setStatusCode(413);
			return (ret);
		}
	}
	if (isDir(path)) {

	}
	
	
	struct stat path_stat;

	if (stat(path.c_str(), &path_stat) == 0) {
		ret.setStatusCode(409);
		return (ret);
	}
	
	return (ret);
}