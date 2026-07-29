/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiChild.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/18 22:41:57 by acamargo          #+#    #+#             */
/*   Updated: 2026/07/29 23:28:22 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Request.hpp"
#include "Server.hpp"
#include "locationConfig.hpp"
#include "send_http_response.hpp"
#include "utils.hpp"
#include "utils_execution.hpp"
#include "utils_logs.hpp"
#include <CgiChild.hpp>
#include <cerrno>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <stdexcept>
#include <string>
#include <unistd.h>
CgiChild::CgiChild(Client& client) : _env(NULL), _args(NULL), _client_owner(client), _pid(-1)
{
	std::time(&this->_last_communication);
	this->_file_path = client.getRequest().getHeader().getTargetResource();
	this->is_done = false;
	this->is_script_running = false;
	this->_read_from_script_pipe[0] = -1;
	this->_read_from_script_pipe[1] = -1;
	this->_write_2_script[0] = -1;
	this->_write_2_script[1] = -1;
	return;
}

CgiChild::CgiChild(CgiChild const& other) : _env(NULL), _args(NULL), _client_owner(other._client_owner)
{
	this->_pid = other._pid;
	this->is_script_running = other.is_script_running;
	this->is_done = other.is_done;
	this->_read_from_script_pipe[0] = other._read_from_script_pipe[0];
	this->_read_from_script_pipe[1] = other._read_from_script_pipe[1];
	this->_write_2_script[0] = other._write_2_script[0];
	this->_write_2_script[1] = other._write_2_script[1];
	this->_file_name = other._file_name;
	this->_file_path = other._file_path;
	this->_extension = other._extension;
	this->_output = other._output;
	this->_last_communication = other._last_communication;
}

CgiChild::~CgiChild()
{
	if (this->_env)
		free_double_char_ptr(this->_env);
	if (this->_args)
		free_double_char_ptr(this->_args);
	close_pipe(this->_read_from_script_pipe);
	close_pipe(this->_write_2_script);
}

CgiChild& CgiChild::operator=(CgiChild const& other)
{
	if (this == &other)
		return *this;
	this->~CgiChild();
	new (this) CgiChild(other);
	return (*this);
}

namespace
{
bool	change_script_name(Request& r, std::string& file_name)
{
	if (!r.getLocConfBlock()->hasIndexCgi())
		return false;
	size_t last_slash_pos = file_name.find_last_of('/');
	file_name.erase(last_slash_pos + 1, std::string::npos);
	file_name.append(r.getLocConfBlock()->getIndexCgi());
	return true;
}

void	find_interpreter(std::string const& extension, std::string& interpreter, locationConfig const& location_conf_block)
{
	std::map<std::string, std::string>::const_iterator	it = location_conf_block.getCGI().find(extension);
	if (it == location_conf_block.getCGI().end())
		return ;
	interpreter = it->second;
	return;
}

}
void	CgiChild::setReadFromScriptPipe(int* pipe_fd)
{
	this->_read_from_script_pipe[0] = pipe_fd[0];
	this->_read_from_script_pipe[1] = pipe_fd[1];
	return;
}

void	CgiChild::setWrite2Script(int* pipe_fd)
{
	this->_write_2_script[0] = pipe_fd[0];
	this->_write_2_script[1] = pipe_fd[1];
	return;
}

int		CgiChild::execute_script(Server& server)
{

	size_t	len = this->_client_owner.getRequest().getBody().length();
	writeall(this->_write_2_script[1], this->_client_owner.getRequest().getBody().c_str(), len);
	if (len != this->_client_owner.getRequest().getBody().length())
		print_log(TEXT_RED, NULL, "Could not send all the body", 1);
	this->fork_child(server);
	this->is_script_running = true;
	return 0;
}

int		CgiChild::set_cgi()
{
	find_extension(this->_file_path, this->_extension);
	find_interpreter(this->_extension, this->_interpreter, *this->_client_owner.getRequest().getLocConfBlock());
	if ((this->_extension.empty() || this->_extension == ".ws") && !change_script_name(this->_client_owner.getRequest(), this->_file_path))
	{
		print_log(TEXT_YELLOW, NULL, "Could not execute cgi, script misssing", 1);
		return not_found;
	}
	size_t	file_name_pos = this->_file_path.find_last_of('/') + 1;
	this->_file_name = "./" + this->_file_path.substr(file_name_pos, std::string::npos);
	this->_file_path.erase(file_name_pos, std::string::npos);
	std::string	full_file_path = this->_file_path + this->_file_name;
	if (access(full_file_path.c_str(), F_OK) != 0)
	{
		print_log(TEXT_YELLOW, NULL, "Could not execute cgi, script misssing", 1);
		return not_found;
	}
	int	access_opts = R_OK;
	if (this->_interpreter.empty())
		access_opts = access_opts | X_OK;
	if (access(full_file_path.c_str(), access_opts) != 0)
	{
		print_log(TEXT_YELLOW, NULL, "Could not execute cgi, permission denied", 1);
		return forbiden;
	}
	this->create_args(this->_interpreter);
	this->create_env(this->_client_owner.getRequest());
	return 0;
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
		this->_args[i] = ft_create_c_str(this->_file_name);
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

int	CgiChild::fork_child(Server& server)
{
	this->_pid = fork();
	if (this->_pid == -1)
		throw std::runtime_error("Fork: " + std::string(strerror(errno)));
	if (this->_pid == 0)
	{
		if (dup2(this->_read_from_script_pipe[1], STDOUT_FILENO) == -1
			|| dup2(this->_write_2_script[0], STDIN_FILENO) == -1)
			throw std::runtime_error("Dup2: " + std::string(strerror(errno)));
		if (!this->_args || !this->_env)
			throw std::runtime_error("execve args empty");
		server.close_server(false);
		close_pipe(this->getWrite2Script());
		close_pipe(this->getReadFromScriptPipe());
		chdir(this->_file_path.c_str());
		execve(this->_args[0], this->_args, this->_env);
		throw std::runtime_error("execve: " + std::string(strerror(errno)));
	}
	close(this->_read_from_script_pipe[1]);
	close(this->_write_2_script[0]);
	this->_write_2_script[0] = -1;
	this->_read_from_script_pipe[1] = -1;
	return 0;
}

char**	CgiChild::getArgs(void) const
{
	return this->_args;
}

char**	CgiChild::getEnv(void) const
{
	return this->_env;
}

std::string const&	CgiChild::getFilename(void) const
{
	return this->_file_name;
}

std::string const&	CgiChild::getFilepath(void) const
{
	return this->_file_path;
}

std::string const&	CgiChild::getExtension(void) const
{
	return this->_extension;
}

std::string&	CgiChild::getOutput(void)
{
	return this->_output;
}

void	CgiChild::appedOutput(std::string const& str)
{
	this->_output.append(str);
}

void	CgiChild::setFilename(std::string const& str)
{
	this->_file_name = str;
}

void	CgiChild::setFilepath(std::string const& str)
{
	this->_file_path = str;
}

void	CgiChild::setExtension(std::string const& str)
{
	this->_extension = str;
}

Client&	CgiChild::getClientOwner(void)
{
	return this->_client_owner;
}

time_t			CgiChild::getLastComm(void) const
{
	return this->_last_communication;
}

void	CgiChild::setLastComm(void)
{
	std::time(&this->_last_communication);
}

int*	CgiChild::getWrite2Script(void)
{
	return this->_write_2_script;
}

int*	CgiChild::getReadFromScriptPipe(void)
{
	return this->_read_from_script_pipe;
}

int		CgiChild::getPid(void) const
{
	return this->_pid;
}
