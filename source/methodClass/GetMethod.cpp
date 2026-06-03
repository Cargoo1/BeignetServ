#include <GetMethod.hpp>

 #include <unistd.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/stat.h>

#define NULL_STR ""

GetMethod::GetMethod(ExecutionContext &context) : HttpMethod(context) {};

GetMethod::~GetMethod() {};

namespace { std::string getQuery(const std::string path) {
	std::size_t found = path.find_first_of("?");
	if (found == std::string::npos) {
		return (NULL_STR);
	}
	std::string query = path.substr(found+1);
	query = query.substr(0, query.find_first_of(" "));
	return (query);
} }

namespace { std::string getPath(const std::string path) {
	std::string newPath = path.substr(path.find_first_of("/"));
	newPath = newPath.substr(0, newPath.find_first_of("?"));
	return (newPath);
} }

namespace { bool isCgi(const std::string path){
	std::string ext = findExt(path);
	if (ext == "cgi")
		return (true);
	return(false);
} }

namespace { char **buildEnv(const ExecutionContext &context, const std::string env){
	std::map<std::string, std::string> mapEnv;
	std::string tmp;
	mapEnv["REQUEST_METHOD"] = context.request.getHeader().getMethod();
	std::string targetR = context.request.getHeader().getTargetResource();
	mapEnv["SCRIPT_NAME"] = getPath(targetR);
	mapEnv["QUERY_STRING"] = getQuery(targetR);
	mapEnv["SERVER_NAME"] = context.server.serverName();
	std::string::size_type pos = context.server.listen().find_last_of(":");
	mapEnv["SERVER_PORT"] = context.server.listen().substr(pos+1);
	mapEnv["SERVER_PROTOCOL"] = context.request.getHeader().getProtocolV();
	mapEnv["GATEWAY_INTERFACE"] = "CGI/1.1";
	mapEnv["SERVER_SOFTWARE"] = "BeignetServ/1.0";
	mapEnv["REMOTE_ADDR"] = ""; //Need ip adress of client
	mapEnv["PATH_INFO"] = "";
	mapEnv["PATH_TRANSLATED"] = "";

} }

namespace { char **builArgs(const std::string args){

} }

void GetMethod::executeMethod(HttpResponse &rsp) {
	if (this->_context.location.hasRedirect()) {
		rsp.setStatusCode(this->_context.location.getRedirectCode());
		rsp.addField("Location", this->_context.location.getRedirectUrl());
		return;
	}
	std::string targetR = this->_context.request.getHeader().getTargetResource();
	std::string query = getQuery(targetR);
	std::string cgiPath = getPath(targetR);
	if (isCgi(cgiPath)) {
		char **envp = buildEnv(this->_context);
		char **av = builArgs(cgiPath);

		pipe();
		fork();
		pid_t pid = getpid();
		if (pid == 0) {
			dup2();
			execve(cgiPath.c_str(), av, envp);
		}
		read();
	}
	std::string root = this->_context.location.getRoot();
	if (!root.empty() && root.back() != '/')
		root += '/';
	std::string path = root + targetR;
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