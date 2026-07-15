/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi_protocol.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/30 19:17:45 by acamargo          #+#    #+#             */
/*   Updated: 2026/06/30 20:54:03 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponse.hpp"
#include "Request.hpp"
#include "locationConfig.hpp"
#include "utils.hpp"

#include <bits/stdc++.h>
#include <vector>
#include <sys/stat.h>
#include <sys/wait.h>
/*
	
*/

namespace { 
	char *ft_dupStrC(const std::string &s) {
		char *dup = new char[s.size() + 1];
		std::memcpy(dup, s.c_str(), s.size() + 1);
		return dup;
	}

	char **buildEnv(Request const& r){
		std::map<std::string, std::string> mapEnv;
		std::string tmp;
		const std::string targetR = r.getHeader().getTargetResource();
		const serverConfig *sb = r.getServerBlock();


		mapEnv["DOCUMENT_ROOT"] = sb->getRoot();

		mapEnv["REQUEST_METHOD"] = r.getHeader().getMethod();
	
		mapEnv["SCRIPT_NAME"] = getQuery_path(targetR);
		mapEnv["QUERY_STRING"] = getQuery(targetR);

		
		std::string::size_type pos = sb->listen().find_last_of(":");
		mapEnv["SERVER_PORT"] = sb->listen().substr(pos+1);

		mapEnv["SERVER_PROTOCOL"] = r.getHeader().getProtocolV();
		mapEnv["GATEWAY_INTERFACE"] = "CGI/1.1";
		mapEnv["SERVER_SOFTWARE"] = "BeignetServ/1.0";
		mapEnv["REMOTE_ADDR"] = ""; //Need ip adress of client
		mapEnv["PATH_INFO"] = "";
		mapEnv["PATH_TRANSLATED"] = "";


		char **ret = new char*[mapEnv.size() + 1];
		std::size_t i = 0;
		std::map<std::string, std::string>::iterator it = mapEnv.begin();
		for (;it != mapEnv.end(); it++) {
			ret[i++] = ft_dupStrC(it->first + "=" + it->second);
		}
		ret[i] = 0;
		return (ret);
	}

	char **builArgs(const std::string args){
			char **ret = new char*[2];
			ret[0] = ft_dupStrC(args);
			ret[1] = 0;
			return ret;
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
}

void	execute_script(Request const& r, HttpResponse& response)
{
	std::string targetR = r.getHeader().getTargetResource();
	std::string query = getQuery(targetR);
	std::string cgiPath = getQuery_path(targetR);
	std::string scriptRet;
	char buffer[4096];
	ssize_t bytesRead;
	int status;

	char **envp = buildEnv(r);
	char **av = builArgs(cgiPath);
	int pipefd[2];

	if (pipe(pipefd) == -1)
		throw Request::ErrorRequest(not_found, "CGI: pipe faild");
	pid_t pid = fork();
	if (pid == 0) {
		dup2(pipefd[1], STDOUT_FILENO);
		execve(cgiPath.c_str(), av, envp);
	}
	else if (pid == -1)
		throw Request::ErrorRequest(not_found, "CGI: pid == -1");
	else {
		close(pipefd[1]);
		while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer))) > 0)
			scriptRet.append(buffer, bytesRead);
		waitpid(pid, &status, 0);
		close(pipefd[0]);
	}
}
