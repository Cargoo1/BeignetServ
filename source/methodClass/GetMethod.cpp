#include <GetMethod.hpp>

#include <limits.h>
#include <stdlib.h>
#include <sys/stat.h>

GetMethod::GetMethod(ExecutionContext &context) : HttpMethod(context) {};

GetMethod::~GetMethod() {};

HttpResponse &GetMethod::executeMethod() {
	HttpResponse ret(200);
	std::string path = NormalizePath(this->_context.location.getRoot()) + NormalizePath(this->_context.location.getPath());
	// std::string path = NormalizePath(this->_context.location.getRoot()) + NormalizePath(this->_context.request.getHeader().getTargetResource());
	struct stat path_stat;
	if (stat(path.c_str(), &path_stat) < 0) {
		ret.setStatusCode(404);
		return (ret);
	}
	if (S_ISREG(path_stat.st_mode)) {
		ret = executeMethod();
	}
}