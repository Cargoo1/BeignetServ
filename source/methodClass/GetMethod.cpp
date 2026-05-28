#include <GetMethod.hpp>

#include <limits.h>
#include <stdlib.h>
#include <sys/stat.h>

GetMethod::GetMethod(ExecutionContext &context) : HttpMethod(context) {};

GetMethod::~GetMethod() {};

void GetMethod::executeMethod(HttpResponse &rsp) {
	std::string path = this->_context.location.getRoot() + this->_context.request.getHeader().getTargetResource();
	// std::string path = NormalizePath(this->_context.location.getRoot()) + NormalizePath(this->_context.location.getPath());
	struct stat path_stat;
	if (stat(path.c_str(), &path_stat) < 0) 
		throw Request::ErrorRequest(not_found);
	if (S_ISDIR(path_stat.st_mode)) {
		path += "/index.html";
		if (stat(path.c_str(), &path_stat) < 0)
			throw Request::ErrorRequest(forbiden);
	}
	if (!rsp.setBodyFromFile(path))
		throw Request::ErrorRequest(not_found);
	rsp.setContentType(this->getContentType(path));
	rsp.addContentLength();
}