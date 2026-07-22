/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 15:15:33 by acamargo          #+#    #+#             */
/*   Updated: 2026/07/22 23:26:45 by acamargo         ###   ########.fr       */
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
#include <cstdlib>
#include <ctime>
#include <exception>
#include <serverConfig.hpp>
#include <iostream>
#include <Server.hpp>
#include <sstream>
#include <stdexcept>
#include <string>
#include <sys/epoll.h>
#include <sys/poll.h>
#include <unistd.h>
#include <utility>
#include <vector>

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

void	Server::addClient(int fd, uint32_t events, std::string const& ip, std::string const& port)
{
	std::string	log;
	std::stringstream	ss;
	this->setEinf(fd, events);
	epoll_ctl(this->_epollfd, EPOLL_CTL_ADD, fd, &this->_einf);
	this->_clients.insert(std::pair<int, Client>(fd, Client(fd, events)));
	this->_clients.at(fd).setIpPort(ip, port);
	ss << fd;
	log = "New client connected to: " + this->_clients.at(fd).getIp() +
				':' + this->_clients.at(fd).getPort() +
				", listen fd: " + ss.str() + '\n';
	print_log(TEXT_GREEN, NULL, log, 0);
}

int	Server::addCgiChild(uint32_t events, Client& client)
{
	std::string	log;
	int	pipe_fd[2];
	if (pipe(pipe_fd) == -1)	
	{
		print_log(TEXT_RED, NULL, strerror(errno), true);
		return errno;
	}
	this->_scripts_childs.insert(std::pair<int, CgiChild>(pipe_fd[0], CgiChild(client)));
	CgiChild&	child = this->_scripts_childs.at(pipe_fd[0]);
	child.setPipe(pipe_fd);
	int	infno = child.execute_cgi();
	if (infno < 0)
	{
		this->close_server(false);
		throw std::exception();
	}
	else if (infno != 0)
	{
		send_response(client.getRequest(), client.getRequest().getResponse(), client.getFd(), infno);
		this->deleteCgiChild(child.getPipe()[0]);
		return -1;
	}
	client.getRequest().is_cgi_in_progress = true;
	this->setEinf(child.getPipe()[0], events);
	epoll_ctl(this->_epollfd, EPOLL_CTL_ADD, child.getPipe()[0], &this->_einf);
	client.getRequest().setPipeFd(child.getPipe()[0]);
	log = "Pipe added to Epoll pool: " + toStr(child.getPipe()[0]) + "\n";
	print_log(TEXT_GREEN, NULL, log, 0);
	return INCOMPLETE;
}

void	Server::deleteClient(int fd, bool remove_from_epoll)
{
	std::string	fd_str;
	ft_itoa(fd, fd_str);
	if (remove_from_epoll)
	{
		print_log(TEXT_YELLOW, NULL, "Closing connection and deleting client: "+
				this->_clients.at(fd)._ip+
				":" +
				this->_clients.at(fd)._port+
				" " + fd_str, 0);
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
	print_log(TEXT_YELLOW, NULL, "Closing and deleting pipe: " + toStr(pipe_fd), 0);
	epoll_ctl(this->_epollfd, EPOLL_CTL_DEL, pipe_fd, &this->_einf);
	this->_scripts_childs.at(pipe_fd).getClientOwner().getRequest().setPipeFd(-1);
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

const SessionManager				&Server::getSession() const {
	return (this->_sessions);
}
