#include "Request.hpp"
#include <GetMethod.hpp>

#include <iostream>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>

GetMethod::GetMethod(Request const& r) : HttpMethod(r) {};

GetMethod::~GetMethod() {};

namespace { bool isCgi(const std::string path){
	std::string ext = findExt(path);
	if (ext == "cgi")
		return (true);
	return(false);
} }

void GetMethod::executeMethod(HttpResponse &rsp) {
	if (this->_request.getLocConfBlock()->hasRedirect()) {
		rsp.setStatusCode(this->_request.getLocConfBlock()->getRedirectCode());
		rsp.addField("Location", this->_request.getLocConfBlock()->getRedirectUrl());
		return;
	}
	std::string path = this->_request.getHeader().getTargetResource();
	std::string query = getQuery(path);
	std::string cgiPath = getQuery_path(path);
	if (isCgi(cgiPath)) {
		// _executeCGI(this->_context);
		return;
	}
	struct stat path_stat;
	if (stat(path.c_str(), &path_stat) < 0) 
		throw Request::ErrorRequest(not_found, "GET: no such file or directory (stat)");
	if (S_ISDIR(path_stat.st_mode)) {
		if (path.at(path.size()-1) != '/') {
			rsp.setStatusCode(301);
			rsp.addField("Location", path + "/");
			return;
		}
		std::string indexPath = path + "index.html";
		if (stat(indexPath.c_str(), &path_stat) == 0 && S_ISREG(path_stat.st_mode)) {
			if (!rsp.setBodyFromFile(indexPath))
				throw Request::ErrorRequest(not_found, "GET: the index.html file does not exist");
			rsp.setContentType(find_content_type(indexPath));
			rsp.addContentLength();
			rsp.setStatusCode(200);
			return;
		}
		if (this->_request.getLocConfBlock()->getAutoindex()) {
			std::string htmlList = _generateAutoindex(path);
			rsp.setBody(htmlList);
			rsp.setContentType("text/html");
			rsp.addContentLength();
			rsp.setStatusCode(200);
			return;
		}
		throw Request::ErrorRequest(forbiden, "GET: does not have the access to the directory");
	}
	else if (!S_ISREG(path_stat.st_mode)) {
		throw Request::ErrorRequest(forbiden, "GET: does not have the access to the file");
	}
	if (!rsp.setBodyFromFile(path))
		throw Request::ErrorRequest(not_found, "GET: the index.html is either empty or does not exist");
	rsp.setContentType(find_content_type(path));
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
		throw Request::ErrorRequest(not_found, "GET: (auto-index) the directory does not exist");
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
