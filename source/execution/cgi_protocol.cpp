/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi_protocol.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/30 19:17:45 by acamargo          #+#    #+#             */
/*   Updated: 2026/07/23 21:16:36 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiChild.hpp"
#include "Client.hpp"
#include "HttpResponse.hpp"
#include "Request.hpp"
#include "Server.hpp"
#include "locationConfig.hpp"
#include "parserUtils.hpp"
#include "send_http_response.hpp"
#include "utils.hpp"
#include "utils_logs.hpp"

#include <bits/stdc++.h>
#include <cerrno>
#include <cstddef>
#include <cstring>
#include <map>
#include <parse_request.hpp>

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

	std::string find_extension(const std::string &cgi, const locationConfig &loc) {
		std::string interpret = cgi.substr(cgi.find_last_of("."));
		std::string ret;
		std::map<std::string, std::string>::const_iterator it = loc.getCGI().find(interpret);
		if (it != loc.getCGI().end())
			ret = it->second;
		else
			ret = "";
		
		// (void)loc;
		// if (interpret == ".py")
		// 	return PYTHON;
		// else if (interpret == ".cgi")
		// 	return CGI_EXT;
		// else if (interpret == ".ws")
		// 	return WS;
		return ret;
	}

	// std::string get_interpret(const std::map<std::string, std::string> &map, const std::string &expr) {
	// 	std::string ret;
	// 	std::map<std::string, std::string>::const_iterator it;
	// 	it = map.find(expr);
	// 	if (it != map.end())
	// 		ret = it->second;
	// 	else
	// 		ret = "";
	// }

	// const std::string define_interpret(const t_scripts_ext ext, const locationConfig *loc) {
	// 	std::string ret;
	// 	std::map<std::string, std::string>::const_iterator it;
	// 	switch (ext)
	// 	{
	// 	case PYTHON:
	// 		ret = "";
	// 		break;
	// 	case CGI_EXT:
	// 		ret = "";
	// 		break;
	// 	case WS:
	// 		ret = "";
	// 		break;
	// 	default:
	// 		ret = "";
	// 		break;
	// 	}
	// 	return (ret);
	// }

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
		if (interpretor == "")
		{
			char **ret = new char*[2];
			(void)interpretor;
			ret[0] = ft_dupStrC(script);
			ret[1] = 0;
			return ret;
		}
		else {
			char **ret = new char*[3];
			ret[0] = ft_dupStrC(interpretor);
			ret[0] = ft_dupStrC(script);
			ret[1] = 0;
			return ret;
		}
	}

}

int	get_script_output(Server& server, int pipe_fd)
{
	std::string	msg;
	CgiChild&	script = server.getCgiChilds().at(pipe_fd);
	Request&	r = script.getClientOwner().getRequest();
	if (read_msg(msg, script.getPipe()[0]) > 0)
	{
		script.setLastComm();
		script.appedOutput(msg);
		return 1;
	}
	Request	cgi_temp_request;
	cgi_temp_request.getHeader().is_method_parsed = true;
	try
	{
		parse_fields(script.getOutput(), cgi_temp_request);
	}
	catch (Request::ErrorRequest& e)
	{
		print_log(TEXT_RED, &e, e.what(), 1);
		send_response(r, r.getResponse(), script.getClientOwner().getFd(), bad_geteway);
		server.deleteCgiChild(pipe_fd);
		return -1;
	}
	waitpid(r.getPipeFd(), NULL, 0);
	r.getResponse().setBody(script.getOutput());
	r.getResponse().setStatusCode(200);
	std::map<std::string, std::string>::iterator it = cgi_temp_request.getHeader().getFields().find("Status");
	if (it != cgi_temp_request.getHeader().getFields().end())
	{
		int	status = toInt(it->second);
		if (status > 505 || status < 200)
			status = 200;
		r.getResponse().setStatusCode(status);
	}
	send_response(r, r.getResponse(), script.getClientOwner().getFd(), 0);
	server.deleteCgiChild(pipe_fd);
	return 0;
}

void	check_idle_scripts(Server& server)
{
	time_t	curr_time = 0;

	std::time(&curr_time);
	if (curr_time - server.getLastCheckScripts() < 1)
		return;
	std::map<int, CgiChild>::const_iterator	it;
	for (it = server.getCgiChilds().begin(); it != server.getCgiChilds().end();)
	{
		if (curr_time - it->second.getLastComm() < 60)
		{
			++it;
			continue;
		}
		print_log(TEXT_YELLOW, NULL, "Removing iddle pipe: " + toStr(it->first), 0);
		server.deleteCgiChild((it++)->first);
	}
	server.setLastCheckScripts();
}

int	execute_script(Request const& r, HttpResponse& response)
{
	std::string script_path = r.getHeader().getTargetResource();
	(void)response;
	std::string query = r.getHeader().getQueryStr();
	//std::string cgiPath = getQuery_path(targetR);
	std::string extension = find_extension(script_path, *r.getLocConfBlock());
	if (extension == "") {
		if (r.getLocConfBlock()->hasIndexCgi()) 
		{
			size_t last_slash_pos = script_path.find_last_of('/');
			script_path.erase(last_slash_pos + 1, std::string::npos);
			script_path.append(r.getLocConfBlock()->getIndexCgi());
		}
		else
			throw Request::ErrorRequest(not_found, "CGI: the script either dosn't exist or a non supported language"); 
	}

	std::string scriptRet;
	//char buffer[4096];
	//ssize_t bytesRead;
	//int status;

	char **env = buildEnv(r);
	char **args = builArgs(script_path, extension);
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
		free_double_char_ptr(env);
		free_double_char_ptr(args);
		exit(1);
	}
	if (pid == -1)
	{
		free_double_char_ptr(env);
		free_double_char_ptr(args);
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
