#include <HttpMethod.hpp>

HttpMethod::HttpMethod(const ExecutionContext &context) : _context(context) {}

bool HttpMethod::verifyPath() {

}

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

}

std::string HttpMethod::getContentType(const std::string &filepath) {

}

void HttpMethod::buildErrorResponse(HttpResponse &res, int code, std::string msg) {

}
