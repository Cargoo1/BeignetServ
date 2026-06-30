/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 15:15:33 by acamargo          #+#    #+#             */
/*   Updated: 2026/06/30 15:56:57 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils_logs.hpp"
#include <Client.hpp>
#include <ctime>
#include <serverConfig.hpp>
#include <iostream>
#include <Server.hpp>
#include <sstream>
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
	this->_epollfd = -1;
	return ;
}

Server::~Server(void)
{
	return;
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
void	Server::deleteClient(int fd)
{
	std::string	fd_str;
	ft_itoa(fd, fd_str);
	print_log(TEXT_YELLOW, NULL, "Closing connection and deleting client: "+
			this->_clients.at(fd)._ip+
			":" +
			this->_clients.at(fd)._port+
			" " + fd_str, 0);
	epoll_ctl(this->_epollfd, EPOLL_CTL_DEL, fd, &this->_einf);
	close(fd);
	this->_clients.erase(fd);
}
