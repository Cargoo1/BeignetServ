/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi_protocol.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/30 19:17:45 by acamargo          #+#    #+#             */
/*   Updated: 2026/07/17 13:33:31 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "HttpResponse.hpp"
#include "Request.hpp"
#include "Server.hpp"
#include "locationConfig.hpp"
#include "send_http_response.hpp"
#include "utils.hpp"
#include "utils_logs.hpp"

#include <bits/stdc++.h>
#include <cerrno>
#include <cstring>
#include <sys/socket.h>
#include <vector>
#include <sys/stat.h>
#include <sys/wait.h>

namespace { 
	char *ft_dupStrC(const std::string &s) {
		char *dup = new char[s.size() + 1];
		std::memcpy(dup, s.c_str(), s.size() + 1);
		return dup;
	}

	std::size_t ft_Dstrlengt (char **str) {
		std::size_t i = 0;
		while (str[i] != 0)
			i++;
		return (i);
	}

	void freeExecVariabl(char **toFree) {
		
		std::size_t length = ft_Dstrlengt(toFree);
		for (std::size_t i = 0; i < length; i++) {
			delete [] toFree[i];
		}
		delete [] toFree;
	}

	t_scripts_ext find_extension(const std::string &cgi, const locationConfig &loc) {
		std::string interpret = cgi.substr(cgi.find_last_of("."));
		(void)loc;
		if (interpret == ".py")
			return PYTHON;
		else if (interpret == ".cgi")
			return CGI_EXT;
		else if (interpret == ".ws")
			return WS;
		return INVALID;
	}

	char **buildEnv(Request const& r){
		std::map<std::string, std::string> mapEnv;
		std::string tmp;
		const std::string targetR = r.getHeader().getTargetResource();
		const serverConfig *server_block = r.getServerBlock();


		mapEnv["DOCUMENT_ROOT"] = server_block->getRoot();
		mapEnv["REQUEST_METHOD"] = r.getHeader().getMethod();
		mapEnv["SCRIPT_NAME"] = r.getHeader().getTargetResource();
		mapEnv["QUERY_STRING"] = r.getHeader().getQueryStr();
		std::string::size_type pos = server_block->listen().find_last_of(":");
		mapEnv["SERVER_PORT"] = server_block->listen().substr(pos+1);
		mapEnv["SERVER_PROTOCOL"] = r.getHeader().getProtocolV();
		mapEnv["GATEWAY_INTERFACE"] = "CGI/1.1";
		mapEnv["SERVER_SOFTWARE"] = "BeignetServ/1.0";
		//mapEnv["REMOTE_ADDR"] = ; //Need ip adress of client
		mapEnv["PATH_INFO"] = r.getHeader().getPathInfo();
		mapEnv["PATH_TRANSLATED"] = "";
		mapEnv["CONTENT_LENGTH"] = toStr(r.getBody().length());
		if (!r.getHeader().getContentType())
			mapEnv["CONTENT_TYPE"] = "";
		else
			mapEnv["CONTENT_TYPE"] = *r.getHeader().getContentType();

		char **ret = new char*[mapEnv.size() + 1];
		std::size_t i = 0;
		std::map<std::string, std::string>::iterator it = mapEnv.begin();
		for (;it != mapEnv.end(); it++) {
			ret[i++] = ft_dupStrC(it->first + "=" + it->second);
		}
		ret[i] = 0;
		return (ret);
	}

	char **builArgs(const std::string &script, const std::string interpretor) {
		char **ret = new char*[2];
		(void)interpretor;
		//ret[0] = ft_dupStrC(interpretor);
		ret[0] = ft_dupStrC(script);
		ret[1] = 0;
		return ret;
	}

}

int	get_script_output(Client& client)
{
	Request&	r = client.getRequest();
	if (!recv_msg(r.getScripOutput(), r.getPipeFd()))
	{
		r.getResponse().setBody(r.getScripOutput());
		send_response(r, r.getResponse(), client.getFd(), 0);
		return 0;
	}
	client.setLastScriptComm();
	return 1;
}

void	check_idle_scripts(Server& server)
{
	time_t	curr_time = 0;

	std::time(&curr_time);
	if (curr_time - server.getLastCheckScripts() < 1)
		return;
	std::map<int, Client*>::const_iterator	it;
	for (it = server.getPipes().begin(); it != server.getPipes().end();)
	{
		if (curr_time - it->second->getLastScriptComm() < 60)
		{
			++it;
			continue;
		}
		print_log(TEXT_YELLOW, NULL, "Removing iddle pipe: " + toStr(it->first), 0);
		server.deletePipe((it++)->first);
	}
	server.setLastCheckScripts();
}

int	execute_script(Request const& r, HttpResponse& response)
{
	std::string script_path = r.getHeader().getTargetResource();
	(void)response;
	std::string query = r.getHeader().getQueryStr();
	//std::string cgiPath = getQuery_path(targetR);
	t_scripts_ext extension = find_extension(script_path, *r.getLocConfBlock());
	(void)extension;
	std::string scriptRet;
	//char buffer[4096];
	//ssize_t bytesRead;
	//int status;

	char **env = buildEnv(r);
	char **args = builArgs(query, "");
	int pipefd[2];

	if (pipe(pipefd) == -1)
		throw Request::ErrorRequest(internal_server_error, "CGI: pipe faild");
	pid_t pid = fork();
	if (pid == 0) {
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[0]);
		close(pipefd[1]);
		execve(args[0], args, env);
		print_log(TEXT_RED, NULL, strerror(errno), true);
		//need to exit properly!!!!!
		freeExecVariabl(args);
		freeExecVariabl(env);
		exit(1);
	}
	if (pid == -1)
	{
		freeExecVariabl(args);
		freeExecVariabl(env);
		throw Request::ErrorRequest(internal_server_error, "CGI: pid == -1");
	}
	close(pipefd[1]);
	return pipefd[0];
	/*
	while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer))) > 0)
		scriptRet.append(buffer, bytesRead);
	waitpid(pid, &status, 0);
	close(pipefd[0]);
	*/
}
