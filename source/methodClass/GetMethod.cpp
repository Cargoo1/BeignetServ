#include <GetMethod.hpp>
#include <limits.h>
#include <stdlib.h>
#include <sys/stat.h>

GetMethod::GetMethod(ExecutionContext &context) : HttpMethod(context) {};

GetMethod::~GetMethod() {};

void GetMethod::executeMethod(HttpResponse &rsp) {
	if (this->_context.location.hasRedirect()) {
		rsp.setStatusCode(this->_context.location.getRedirectCode());
		rsp.addField("Location", this->_context.location.getRedirectUrl());
		return;
	}
	std::string root = this->_context.location.getRoot();
	if (!root.empty() && root.back() != '/')
		root += '/';
	std::string path = root + this->_context.request.getHeader().getTargetResource();
	struct stat path_stat;
	if (stat(path.c_str(), &path_stat) < 0) 
		throw Request::ErrorRequest(not_found);

	if (S_ISDIR(path_stat.st_mode)) {
		path += "index.html";
		if (stat(path.c_str(), &path_stat) < 0)
			throw Request::ErrorRequest(not_found);  // ← Changé: forbiden → not_found
		if (!S_ISREG(path_stat.st_mode))
			throw Request::ErrorRequest(forbiden);
	}
	else if (!S_ISREG(path_stat.st_mode)) {
		throw Request::ErrorRequest(forbiden);
	}
	if (!rsp.setBodyFromFile(path))
		throw Request::ErrorRequest(not_found);
	rsp.setContentType(this->getContentType(path));
	rsp.addContentLength();
	rsp.setStatusCode(200);
}