/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 15:15:33 by acamargo          #+#    #+#             */
/*   Updated: 2026/05/19 20:38:51 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Client.hpp>
#include "configClass/serverConfig.hpp"
#include <Server.hpp>
#include <sys/epoll.h>
#include <sys/poll.h>
#include <vector>

Server::Server(Server const& other) : _server_conf(other._server_conf)
{
	this->_einf = other._einf;
	this->_sfds = other._sfds;
	this->_epollfd = other._epollfd;
}

Server::Server(std::vector<serverConfig> const& servers_conf) : _server_conf(servers_conf)
{
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

void						Server::setEinf(int fd, uint32_t events)
{
	this->_einf.data.fd = fd;
	this->_einf.events = events;
}

struct epoll_event*			Server::getEventQueue(void)
{
	return this->_eventQueue;
}

std::vector<Client>&	Server::getClients(void)
{
	return this->_clients;
}

void	Server::addClient(int fd, uint32_t events)
{
	this->setEinf(fd, EPOLLIN);
	epoll_ctl(this->_epollfd, EPOLL_CTL_ADD, fd, &this->_einf);
	this->_clients.push_back(Client(fd, events));
}
