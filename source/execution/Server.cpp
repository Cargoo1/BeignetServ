/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 15:15:33 by acamargo          #+#    #+#             */
/*   Updated: 2026/05/14 20:39:41 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Client.hpp>
#include "configClass/serverConfig.hpp"
#include <Server.hpp>
#include <sys/poll.h>
#include <vector>

Server::Server(Server const& other) : _server_conf(other._server_conf)
{
	this->_clients = other._clients;
	this->_connections = other._connections;
	this->_sfd = other._sfd;
	return;
}

Server::Server(serverConfig const& server_conf, int sfd, short events) : _server_conf(server_conf)
{
	this->_sfd = sfd;
	struct pollfd	pfd_tmp = {sfd, events, 0};
	this->_connections.push_back(pfd_tmp);
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

std::vector<struct pollfd>&	Server::getPfd(void)
{
	return this->_connections;
}

int	Server::getSfd(void)
{
	return this->_sfd;
}

std::vector<Client>&	Server::getClients(void)
{
	return this->_clients;
}

void	Server::addPfd(int pfd, short events)
{
	struct pollfd	pfd_tmp = {pfd, events, 0};
	this->_connections.push_back(pfd_tmp);
	return ;
}

bool	Server::addClient(int fd)
{
	if (fd <= 0)
		return false;
	this->_clients.push_back(Client(fd));
	this->addPfd(fd, POLLIN | POLLOUT);
	return true;
}
