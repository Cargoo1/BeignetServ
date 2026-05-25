#include <GetMethod.hpp>

#include <limits.h>
#include <stdlib.h>
#include <sys/stat.h>

GetMethod::GetMethod(ExecutionContext &context) : HttpMethod(context) {};

GetMethod::~GetMethod() {};

HttpResponse &GetMethod::executeMethod() {
	HttpResponse ret(200);
	std::string path = NormalizePath(this->_context.location.getRoot()) + NormalizePath(this->_context.request.getHeader().getTargetResource());
	// std::string path = NormalizePath(this->_context.location.getRoot()) + NormalizePath(this->_context.location.getPath());
	struct stat path_stat;
	if (stat(path.c_str(), &path_stat) < 0) {
		ret.setStatusCode(404);
		return (ret);
	}
	if (S_ISDIR(path_stat.st_mode)) {
		path += "/index.html";
		if (stat(path.c_str(), &path_stat) < 0) {
			ret.setStatusCode(404);
			return (ret);
		}
	}
	if (!ret.setBodyFromFile(path)) {
		ret.setStatusCode(403);
		return (ret);
	}
	ret.setContentType(this->getContentType(path));
	ret.addContentLength();
	return (ret);
}