/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 15:15:33 by acamargo          #+#    #+#             */
/*   Updated: 2026/07/29 23:53:08 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiChild.hpp"
#include "Request.hpp"
#include "SessionManager.hpp"
#include "run_server.hpp"
#include "send_http_response.hpp"
#include "utils.hpp"
#include "utils_execution.hpp"
#include "utils_logs.hpp"
#include <Client.hpp>
#include <cerrno>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <exception>
#include <map>
#include <serverConfig.hpp>
#include <iostream>
#include <Server.hpp>
#include <sstream>
#include <stdexcept>
#include <string>
#include <sys/epoll.h>
#include <sys/poll.h>
#include <sys/wait.h>
#include <unistd.h>
#include <utility>
#include <vector>
#include "signal.h"

Server::Server(Server const& other) : _server_conf(other._server_conf)
{
	this->_einf = other._einf;
	this->_sfds = other._sfds;
	this->_epollfd = other._epollfd;
}

Server::Server(std::vector<serverConfig> const& servers_conf) : _server_conf(servers_conf)
{
	std::time(&this->_last_check);
	std::time(&this->_last_check_scripts);
	this->_epollfd = -1;
	return ;
}

Server::~Server(void)
{
	return;
}

void	Server::close_server(bool remove_from_epoll)
{
	this->close_server_sockets();
	this->close_all_clients(remove_from_epoll);
	close(this->_epollfd);
}

void	Server::close_all_clients(bool remove_from_epoll)
{
	std::map<int, Client>::iterator	it;
	for (it = this->_clients.begin(); it != this->_clients.end();)
		this->deleteClient((it++)->first, remove_from_epoll);
}

void	Server::close_server_sockets(void)
{
	std::vector<int>::iterator	it_sfds;
	std::vector<Client>::iterator	it_clients;

	for (it_sfds = this->_sfds.begin(); it_sfds != this->_sfds.end(); it_sfds++)
		close(*it_sfds);
}

Server&	Server::operator=(Server const& other)
{
	if (this == &other)
		return *this;
	this->~Server();
	new (this) Server(other);
	return *this;
}

void	Server::setEpollfd(int fd)
{
	this->_epollfd = fd;
}

std::vector<int>&	Server::getSfds(void)
{
	return this->_sfds;
}

int	Server::getEpollfd(void)
{
	return this->_epollfd;
}


struct epoll_event&	Server::getEinf(void)
{
	return this->_einf;
}
std::vector<serverConfig> const&	Server::getServerConf(void) const
{
	return this->_server_conf;
}

void						Server::setEinf(int fd, uint32_t events)
{
	this->_einf.data.fd = fd;
	this->_einf.events = events;
}

struct epoll_event*			Server::getEventQueue(void)
{
	return this->_eventQueue;
}

time_t	Server::getLastCheck(void) const
{
	return this->_last_check;
}

void	Server::setLastCheck(void)
{
	std::time(&this->_last_check);
}

std::map<int, Client>&	Server::getClients(void)
{
	return this->_clients;
}

void	Server::set_2_epoll(uint32_t events, int fd, int op) {
	this->setEinf(fd, events);
	if (epoll_ctl(this->_epollfd, op, fd, &this->_einf) == -1)
	{
		close(fd);
		throw std::runtime_error("Epoll: " + std::string(strerror(errno)));
	}
}

void	Server::addClient(int fd, uint32_t events, std::string const& ip, std::string const& port)
{
	std::string	log;
	std::stringstream	ss;
	this->set_2_epoll(events, fd, EPOLL_CTL_ADD);
	this->_clients.insert(std::pair<int, Client>(fd, Client(fd, events)));
	this->_clients.at(fd).setIpPort(ip, port);
	ss << fd;
	log = "New client connected to: " + this->_clients.at(fd).getIp() +
				':' + this->_clients.at(fd).getPort() +
				", listen fd: " + ss.str() + '\n';
	print_log(TEXT_GREEN, NULL, log, 0);
}

int	Server::addCgiChild(Client& client)
{
	std::string	log;
	int	output_pipe[2];
	int	input_pipe[2];

	if (pipe(output_pipe) == -1)	
	{
		print_log(TEXT_RED, NULL, strerror(errno), true);
		throw std::runtime_error("Pipe: " + std::string(strerror(errno)));
	}
	if (pipe(input_pipe) == -1)
	{
		close(output_pipe[0]);
		close(output_pipe[1]);
		throw std::runtime_error("Pipe: " + std::string(strerror(errno)));
	}
	this->_scripts_childs.insert(std::pair<int, CgiChild>(output_pipe[0], CgiChild(client)));
	CgiChild&	child = this->_scripts_childs.at(output_pipe[0]);
	child.setReadFromScriptPipe(output_pipe);
	child.setWrite2Script(input_pipe);
	int	infno = child.set_cgi();
	if (infno != 0)
	{
		child.getClientOwner().getRequest().getResponse().setStatusCode(infno);
		child.getClientOwner().error_request = true;
		this->deleteCgiChild(child.getReadFromScriptPipe()[0]);
		return -1;
	}
	this->_input_pipes.insert(std::pair<int, CgiChild*>(input_pipe[1], &child));
	this->set_2_epoll(EPOLLOUT, child.getWrite2Script()[1], EPOLL_CTL_ADD);
	this->set_2_epoll(EPOLLIN, child.getReadFromScriptPipe()[0], EPOLL_CTL_ADD);
	log = "Child added to Epoll pool: " + toStr(child.getReadFromScriptPipe()[0]) + "\n";
	print_log(TEXT_GREEN, NULL, log, 0);
	client.is_cgi_in_progress = true;
	client.getRequest().setCurrentCgiPipeFd(child.getReadFromScriptPipe()[0]);
	return INCOMPLETE;
}

void	Server::deleteClient(int fd, bool remove_from_epoll)
{
	Client&	client = this->_clients.at(fd);
	if (client._r.getCurrentCgiPipeFd() > -1)
		this->removeChild(client._r.getCurrentCgiPipeFd());
	if (remove_from_epoll)
	{
		print_log(TEXT_YELLOW, NULL, "Closing connection and deleting client: "+
				this->_clients.at(fd)._ip+
				":" +
				this->_clients.at(fd)._port+
				" " + toStr(fd), 0);
		epoll_ctl(this->_epollfd, EPOLL_CTL_DEL, fd, &this->_einf);
	}
	close(fd);
	this->_clients.erase(fd);
}

std::map<int, CgiChild>&	Server::getCgiChilds(void)
{
	return this->_scripts_childs;
}

void	Server::deleteCgiChild(int pipe_fd)
{
	if (this->_scripts_childs.find(pipe_fd) == this->_scripts_childs.end())
		return;
	print_log(TEXT_YELLOW, NULL, "Removing cgi child from server, fd: " + toStr(pipe_fd), true);
	this->_scripts_childs.at(pipe_fd).getClientOwner().is_cgi_in_progress = false;
	epoll_ctl(this->_epollfd, EPOLL_CTL_DEL, this->_scripts_childs.at(pipe_fd).getWrite2Script()[1], &this->_einf);
	this->_input_pipes.erase(this->_scripts_childs.at(pipe_fd).getWrite2Script()[1]);
	close_pipe(this->_scripts_childs.at(pipe_fd).getWrite2Script());
	epoll_ctl(this->_epollfd, EPOLL_CTL_DEL, pipe_fd, &this->_einf);
	this->_scripts_childs.at(pipe_fd).getClientOwner().getRequest().setCurrentCgiPipeFd(-1);
	close_pipe(this->_scripts_childs.at(pipe_fd).getReadFromScriptPipe());
	this->_scripts_childs.erase(pipe_fd);
}

time_t	Server::getLastCheckScripts(void) const
{
	return this->_last_check_scripts;
}

void	Server::setLastCheckScripts(void)
{
	std::time(&this->_last_check_scripts);
}

SessionManager				&Server::getSession() {
	return (this->_sessions);
}

std::map<int, CgiChild*>&	Server::getInputPipes(void)
{
	return this->_input_pipes;
}

const SessionManager				&Server::getSession() const {
	return (this->_sessions);
}

void	Server::remove_from_epoll(int fd)
{
	std::map<int, Client>::iterator it_clients = this->_clients.find(fd);
	if (it_clients != this->_clients.end())
	{
		this->deleteClient(fd, true);
		return;
	}
	return;
}

void	Server::removeChild(int pipe_fd)
{
	if (this->_scripts_childs.find(pipe_fd) == this->_scripts_childs.end())
		return;
	if (this->_scripts_childs.at(pipe_fd).getPid() == 0)
		return ;
	CgiChild& toDelete = this->_scripts_childs.at(pipe_fd);
	int	process_status = 0;
	int	return_value = 0;
	return_value = waitpid(toDelete.getPid(), &process_status, WNOHANG);
	if (return_value > 0 || errno == ECHILD)
	{
		deleteCgiChild(toDelete.getReadFromScriptPipe()[0]);
		return;
	}
	print_log(TEXT_YELLOW, NULL, "Killing child: fd: "
								+ toStr(pipe_fd) + " pid: "
								+ toStr(toDelete.getPid()), 1);
	if (kill(toDelete.getPid(), SIGKILL) > 0)
		print_log(TEXT_RED, NULL, "Could not kill process: " + toStr(toDelete.getPid()) + "!!!", true);
	deleteCgiChild(toDelete.getReadFromScriptPipe()[0]);
}

int		Server::check_pending_requests()
{
	std::map<int, Client>::iterator	it = this->_clients.begin();
	for (; it != this->_clients.end(); ++it)
	{
		if (!it->second._message.empty() && !it->second.is_cgi_in_progress)
			handle_request(it->second, *this);
	}
	return 0;
}

void	Server::check_iddle_clients()
{
	time_t	curr_time;

	std::time(&curr_time);
	if (curr_time - this->_last_check < 1)
		return;
	std::map<int, Client>::iterator	it;
	for (it = this->_clients.begin(); it != this->_clients.end();)
	{
		if (curr_time - it->second.getLastComm() < CLIENT_TIMEOUT || it->second.is_cgi_in_progress)
		{
			++it;
			continue;
		}
		print_log(TEXT_YELLOW, NULL, "Removing iddle client: "
								+ it->second.getIp() + ":"
								+ it->second.getPort() + ", fd: "
								+ toStr(it->first), 0);
		this->deleteClient((it++)->first, true);
	}
	this->setLastCheck();
}

void	Server::check_iddle_cgi_childs()
{
	time_t	curr_time = 0;

	std::time(&curr_time);
	if (curr_time - this->_last_check_scripts < 1)
		return;
	std::map<int, CgiChild>::iterator	it;
	for (it = this->_scripts_childs.begin(); it != this->_scripts_childs.end();)
	{
		if (!it->second.is_done)
		{
			if (curr_time - it->second.getLastComm() < SCRIPT_TIMEOUT)
			{
				++it;
				continue;
			}
			this->set_2_epoll(EPOLLIN | EPOLLOUT, it->second.getClientOwner().getFd(), EPOLL_CTL_MOD);
			it->second.getClientOwner().getRequest().getResponse().setStatusCode(bad_geteway);
			it->second.getClientOwner().error_request = true;
		}
		else
		{
			int return_value = waitpid(it->second.getPid(), NULL, WNOHANG);
			if (return_value > 0 || errno == ECHILD)
			{
				this->deleteCgiChild((it++)->first);
				continue ;
			}
			time_t curr;
			std::time(&curr);
			if (curr - it->second.getLastComm() < PROCESS_TIMEOUT)
			{
				++it;
				continue;
			}

		}
		print_log(TEXT_YELLOW, NULL, "Removing iddle child from this: " + toStr(it->first), 0);
		this->removeChild((it++)->first);
	}
	this->setLastCheckScripts();
}
