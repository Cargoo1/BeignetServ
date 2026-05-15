/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 15:15:33 by acamargo          #+#    #+#             */
/*   Updated: 2026/05/15 21:00:06 by acamargo         ###   ########.fr       */
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
	this->_e_sfds_inf = other._e_sfds_inf;
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

void	Server::addE_sfds_inf(int fd, uint32_t events)
{
	struct epoll_event	ev_temp;

	ev_temp.events = events;
	ev_temp.data.fd = fd;
	this->_e_sfds_inf.push_back(ev_temp);
	return ;
}
