#include "Request.hpp"
#include "utils.hpp"
#include <GetMethod.hpp>
#include <Server.hpp>

#include <ctime>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>

namespace {
	const std::string kCounterRoute = "/counter";
	const std::string kSessionCookieName = "beignet_session";
	const std::string kCounterPlaceholder = "{{COUNTER}}";

	std::string trim(std::string value)
	{
		std::string::size_type start = value.find_first_not_of(" \t");
		std::string::size_type end = value.find_last_not_of(" \t");
		if (start == std::string::npos || end == std::string::npos)
			return "";
		return value.substr(start, end - start + 1);
	}

	std::string extractCookieValue(std::string const& cookieHeader, std::string const& cookieName)
	{
		std::string::size_type start = 0;
		while (start < cookieHeader.size())
		{
			std::string::size_type end = cookieHeader.find(';', start);
			std::string token = trim(cookieHeader.substr(start, end == std::string::npos ? std::string::npos : end - start));
			std::string::size_type separator = token.find('=');
			if (separator != std::string::npos && token.substr(0, separator) == cookieName)
				return trim(token.substr(separator + 1));
			if (end == std::string::npos)
				break;
			start = end + 1;
		}
		return "";
	}

	std::string extractAction(Request const& request)
	{
		std::string query = getQuery(request.getHeader().getTargetResource() + "?" + request.getHeader().getQueryStr());
		std::string::size_type actionPos = query.find("action=");
		if (actionPos == std::string::npos)
			return "";
		return query.substr(actionPos + 7);
	}

	std::string createSessionId()
	{
		static unsigned long long sequence = 0;
		std::ostringstream stream;
		stream << std::time(0) << '-' << ++sequence;
		return stream.str();
	}

	std::string loadTemplate(std::string const& filePath)
	{
		std::ifstream file(filePath.c_str());
		if (!file.is_open())
			return "";
		std::ostringstream buffer;
		buffer << file.rdbuf();
		return buffer.str();
	}

	std::string renderCounterPage(std::string const& filePath, unsigned int counter)
	{
		std::string page = loadTemplate(filePath);
		if (page.empty())
			return page;
		std::string::size_type pos = page.find(kCounterPlaceholder);
		if (pos != std::string::npos)
			page.replace(pos, kCounterPlaceholder.size(), toStr(counter));
		return page;
	}
}

GetMethod::GetMethod(Request const& r, Server& server) : HttpMethod(r), _server(server) {};

GetMethod::~GetMethod() {};

/*
namespace { bool isCgi(const std::string path){
	std::string ext = findExt(path);
	if (ext == "cgi")
		return (true);
	return(false);
} }
*/

void GetMethod::executeMethod(HttpResponse &rsp) {
	if (this->_request.getLocConfBlock()->hasRedirect()) {
		rsp.setStatusCode(this->_request.getLocConfBlock()->getRedirectCode());
		rsp.addField("Location", this->_request.getLocConfBlock()->getRedirectUrl());
		return;
	}
	std::string path = this->_request.getHeader().getTargetResource();
	struct stat path_stat;
	if (stat(path.c_str(), &path_stat) < 0) 
		throw Request::ErrorRequest(not_found, "GET: no such file or directory (stat)");
	if (S_ISDIR(path_stat.st_mode)) {
		if (this->_request.getLocConfBlock()->hasIndex()) {
			std::string indexPath = path + "/" + this->_request.getLocConfBlock()->getIndex();
			if (stat(indexPath.c_str(), &path_stat) == 0) {
				if (S_ISREG(path_stat.st_mode)) {
					if (this->_request.getLocConfBlock()->getPath() == kCounterRoute) {
						std::string sessionId = extractCookieValue(this->_request.getHeader().getCookie(), kSessionCookieName);
						if (sessionId.empty())
							sessionId = createSessionId();
						std::string action = extractAction(this->_request);
						SessionManager& sessions = _server.getSession();
						if (action == "inc")
							sessions.incrCounter(sessionId);
						else if (action == "reset")
							sessions.restCounter(sessionId);
						else
							(void)sessions.getUserCounter(sessionId);
						unsigned int counter = sessions.getUserCounter(sessionId);
						std::string page = renderCounterPage(indexPath, counter);
						if (page.empty())
							throw Request::ErrorRequest(not_found, "GET: the counter page does not exist");
						rsp.setBody(page);
						rsp.addField("Set-Cookie", kSessionCookieName + "=" + sessionId + "; Path=/counter; HttpOnly; SameSite=Lax");
					}
					else if (!rsp.setBodyFromFile(indexPath))
						throw Request::ErrorRequest(not_found, "GET: the index.html file does not exist");
				rsp.setContentType(find_content_type(indexPath));
				rsp.addContentLength();
				rsp.setStatusCode(200);
				}
			return;
		}
		}
		if (this->_request.getLocConfBlock()->getAutoindex()) {
			std::string htmlList = _generateAutoindex(path);
			rsp.setBody(htmlList);
			rsp.setContentType("text/html");
			rsp.addContentLength();
			rsp.setStatusCode(200);
			return;
		}
		throw Request::ErrorRequest(not_found, "GET: does not have the access to the directory");
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
