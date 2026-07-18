/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiChild.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/18 22:41:57 by acamargo          #+#    #+#             */
/*   Updated: 2026/07/19 00:09:28 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include "utils.hpp"
#include "utils_logs.hpp"
#include <CgiChild.hpp>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
CgiChild::CgiChild() : _env(NULL), _args(NULL), _pid(-1)
{
	return;
}

CgiChild::CgiChild(CgiChild const& other)
{
	this->_pid = other._pid;
	this->_pipe_fd[0] = other._pipe_fd[0];
	this->_pipe_fd[1] = other._pipe_fd[1];
	this->_file_name = other._file_name;
	this->_file_path = other._file_path;
	this->_extension = other._extension;
	this->_output = other._output;
}

CgiChild::~CgiChild()
{
	if (this->_env)
		free_double_char_ptr(this->_env);
	if (this->_args)
		free_double_char_ptr(this->_args);
}

CgiChild& CgiChild::operator=(CgiChild const& other)
{
	if (this == &other)
		return *this;
	this->~CgiChild();
	new (this) CgiChild(other);
	return (*this);
}
	
int		CgiChild::create_args(std::string const& interpreter)
{
	size_t	args_len = 2;
	if (!interpreter.empty())
		++args_len;
	this->_args = new char*[args_len];
	for (size_t i = 0; i < args_len - 1; ++i)
	{
		if (i == 0 && !interpreter.empty())
		{
			this->_args[i] = ft_create_c_str(interpreter);
			continue;
		}
		this->_args[i] = ft_create_c_str(this->_file_path);
	}
	this->_args[args_len - 1] = NULL;
	return 0;
}

int		CgiChild::create_env(Request const& r)
{
	std::map<std::string, std::string> map_env;
	std::string tmp;
	const serverConfig *server_block = r.getServerBlock();

	map_env["DOCUMENT_ROOT"] = server_block->getRoot();
	map_env["REQUEST_METHOD"] = r.getHeader().getMethod();
	map_env["SCRIPT_NAME"] = r.getHeader().getTargetResource();
	map_env["QUERY_STRING"] = r.getHeader().getQueryStr();
	//std::string::size_type pos = server_block->listen().find_last_of(":");
	map_env["SERVER_PORT"] = "";//server_block->listen().substr(pos+1);
	map_env["SERVER_PROTOCOL"] = r.getHeader().getProtocolV();
	map_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	map_env["SERVER_SOFTWARE"] = "BeignetServ/1.0";
	//mapEnv["REMOTE_ADDR"] = ; //Need ip adress of client
	map_env["PATH_INFO"] = r.getHeader().getPathInfo();
	map_env["PATH_TRANSLATED"] = this->_file_path;
	map_env["CONTENT_LENGTH"] = toStr(r.getBody().length());
	map_env["CONTENT_TYPE"] = "";
	if (r.getHeader().getContentType())
		map_env["CONTENT_TYPE"] = *r.getHeader().getContentType();
	this->_env = new char*[map_env.size() + 1];
	std::map<std::string, std::string>::iterator it = map_env.begin();
	for (size_t i = 0; it != map_env.end(); ++it, ++i)
		this->_env[i] = ft_create_c_str(it->first + "=" + it->second);
	this->_env[map_env.size()] = NULL;
	return 0;
}

void	CgiChild::fork_child()
{
	if (this->_pipe_fd[1] < 0)
		throw Request::ErrorRequest(internal_server_error, "Unitialized pipe");
	this->_pid = fork();
	if (this->_pid == -1)
		throw Request::ErrorRequest(internal_server_error, "Fork failed");
	if (this->_pid == 0)
	{
		if (dup2(this->_pipe_fd[1], STDOUT_FILENO) == -1)
		{
			print_log(TEXT_RED, NULL, strerror(errno), true);
			exit(errno);
		}

	}
}
	char const**	getArgs(void);
	char const**	getEnv(void);
	std::string const&	getFilename(void);
	std::string const&	getFilepath(void);
	std::string const&	getExtension(void);
	std::string const&	getOutput(void);
	void	appedOutput(std::string const& str);
	void	setFilename(std::string const& str);
	void	setFilepath(std::string const& str);
	void	setExtension(std::string const& str);
