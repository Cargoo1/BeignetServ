#include <HttpMethod.hpp>

#include <bits/stdc++.h>
#include <vector>
#include <sys/stat.h>
#include <sys/wait.h>


// namespace { std::string findRootDir(std::string path) {
// 	std::string del = "/";
// 	std::string::size_type pos = path.find(del);
// 	if (path.substr(0, pos) == "") {
// 		path.erase(0, pos + del.length());
// 		pos = path.find(del);
// 	}
// 	return (path.substr(0, pos));
// } }

namespace { char **buildEnv(const ExecutionContext &context){
	std::map<std::string, std::string> mapEnv;
	std::string tmp;
	mapEnv["REQUEST_METHOD"] = context.request.getHeader().getMethod();
	std::string targetR = context.request.getHeader().getTargetResource();
	mapEnv["SCRIPT_NAME"] = getQuery_path(targetR);
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
	char **ret 

} }

namespace { char **builArgs(const std::string args){
	char **ret = new
} }

HttpMethod::HttpMethod(const ExecutionContext &context) : _context(context) {}

std::string HttpMethod::_NormalizePath(std::string toNomalize) {
	std::vector<std::string> vectStr;
	std::string tmp;
	for (std::size_t i = 0; i < toNomalize.size(); i++) {
		if (toNomalize.at(i) != '/')
			tmp += toNomalize.at(i);
		else {
			if (tmp == ".." && vectStr.size() > 0)
				vectStr.pop_back();
			else if (tmp != "")
				vectStr.push_back(tmp);
			tmp.clear();
		}
	}
	if (tmp == ".." && vectStr.size() > 0)
		vectStr.pop_back();
	else if (tmp != "")
		vectStr.push_back(tmp);
	tmp.clear();
	for (std::vector<std::string>::iterator it = vectStr.begin(); it != vectStr.end(); ++it) {
		tmp += "/" + *it;
	}
	return (tmp);
}

struct stat HttpMethod::_getFileData(const std::string &filePath) {
	struct stat buffer;
	stat(filePath.c_str(), &buffer);
	return (buffer); 
}

std::string HttpMethod::_getContentType(const std::string &filePath) {
	MIME ext = find_type(filePath);
	switch (ext)
	{
		case JPG:
			return ("image/jpeg");
		case PNG:
			return ("image/png");
		case HTML:
			return ("text/html");
		case TXT:
			return ("text/plain");
		default:
			return ("application/octet-stream");
	}
}

// HttpResponse HttpMethod::_buildErrorResponse(HttpResponse &res, int code, std::string msg) {
// 	HttpResponse ret(code);
	
// }

void HttpMethod::_executeCGI(const ExecutionContext context) {
	std::string targetR = this->_context.request.getHeader().getTargetResource();
	std::string query = getQuery(targetR);
	std::string cgiPath = getQuery_path(targetR);
	std::string scriptRet;
	char buffer[4096];
	ssize_t bytesRead;
	int status;

	char **envp = buildEnv(context);
	char **av = builArgs(cgiPath);
	int pipefd[2];

	if (pipe(pipefd) == -1)
		throw Request::ErrorRequest(internal_server_error);
	pid_t pid = fork();
	if (pid == 0) {
		dup2(pipefd[1], STDOUT_FILENO);
		execve(cgiPath.c_str(), av, envp);
	}
	else if (pid == -1)
		throw Request::ErrorRequest(internal_server_error);
	else {
		close(pipefd[1]);
		while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer))) > 0)
			scriptRet.append(buffer, bytesRead);
		waitpid(pid, &status, 0);
		close(pipefd[0]);
	}

}