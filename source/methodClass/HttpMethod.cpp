#include <HttpMethod.hpp>

HttpMethod::HttpMethod(const ExecutionContext &context) : _context(context) {}

bool HttpMethod::verifyContentLength() {

}

bool HttpMethod::verifyBodySize() {

}

bool HttpMethod::checkAllowedMethods() {

}

bool HttpMethod::checkResourceExists() {

}

bool HttpMethod::checkFilePermissions() {

}

std::string HttpMethod::resolveResourcePath() {
	std::string requestPath(this->_context.request.getHeader().getTargetResource());

	
	if (!isPathInsideRoot())
		requestPath.clear();
	return (requestPath)
}

std::string HttpMethod::getContentType(const std::string &filepath) {

}

bool HttpMethod::isPathInsideRoot() {
	std::string root(this->_context.location.getPath());
	std::string resourcePath(resolveResourcePath());
}

void HttpMethod::buildErrorResponse(HttpResponse &res, int code, std::string msg) {

}
