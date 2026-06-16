#include <GetMethod.hpp>

 #include <unistd.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>

GetMethod::GetMethod(ExecutionContext &context) : HttpMethod(context) {};

GetMethod::~GetMethod() {};

namespace { bool isCgi(const std::string path){
	std::string ext = findExt(path);
	if (ext == "cgi")
		return (true);
	return(false);
} }

void GetMethod::executeMethod(HttpResponse &rsp) {
	if (this->_context.location.hasRedirect()) {
		rsp.setStatusCode(this->_context.location.getRedirectCode());
		rsp.addField("Location", this->_context.location.getRedirectUrl());
		return;
	}
	std::string targetR = this->_context.request.getHeader().getTargetResource();
	std::string query = getQuery(targetR);
	std::string cgiPath = getQuery_path(targetR);
	if (isCgi(cgiPath)) {
		// _executeCGI(this->_context);
		return;
	}
	std::string root = this->_context.location.getRoot();
	std::string path = root + targetR;
	struct stat path_stat;
	if (stat(path.c_str(), &path_stat) < 0) 
		throw Request::ErrorRequest(not_found);
	if (S_ISDIR(path_stat.st_mode)) {
		if (targetR.at(targetR.size()-1) != '/') {
			rsp.setStatusCode(301);
			rsp.addField("Location", targetR + "/");
			return;
		}
		std::string indexPath = path + "index.html";
		if (stat(indexPath.c_str(), &path_stat) == 0 && S_ISREG(path_stat.st_mode)) {
			if (!rsp.setBodyFromFile(indexPath))
				throw Request::ErrorRequest(not_found);
			rsp.setContentType(this->_getContentType(indexPath));
			rsp.addContentLength();
			rsp.setStatusCode(200);
			return;
		}
		if (this->_context.location.getAutoindex()) {
			std::string htmlList = _generateAutoindex(path);
			rsp.setBody(htmlList);
			rsp.setContentType("text/html");
			rsp.addContentLength();
			rsp.setStatusCode(200);
			return;
		}
		throw Request::ErrorRequest(forbiden);
	}
	else if (!S_ISREG(path_stat.st_mode)) {
		throw Request::ErrorRequest(forbiden);
	}
	if (!rsp.setBodyFromFile(path))
		throw Request::ErrorRequest(not_found);
	rsp.setContentType(this->_getContentType(path));
	rsp.addContentLength();
	rsp.setStatusCode(200);
}

std::string GetMethod::_generateAutoindex(const std::string &path) {
	std::string html = "<!DOCTYPE html>\n<html>\n<head>\n";
	html += "<title>Index of " + path + "</title>\n";
	html += "</head>\n<body>\n";
	html += "<h1>Index of " + path + "</h1>\n";
	html += "<ul>\n";

	DIR *dir = opendir(path.c_str());
	if (!dir)
		throw Request::ErrorRequest(not_found);
	struct dirent *entry;
	std::vector<std::string> entries;
	while ((entry = readdir(dir))) {
		std::string name = entry->d_name;
		if (name == "." || name == "..")
			continue;
		entries.push_back("<li><a href=\"" + name + "\">" + name + "</a></li>\n");
	}
	closedir(dir);
	for (size_t i = 0; i < entries.size(); ++i)
		html += entries[i];
	html += "</ul>\n</body>\n</html>\n";
	return html;
}