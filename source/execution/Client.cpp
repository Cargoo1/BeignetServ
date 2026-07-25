/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/14 19:50:59 by acamargo          #+#    #+#             */
/*   Updated: 2026/07/25 14:43:47 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Client.hpp>
#include <Request.hpp>
#include <iostream>

Client::Client(int fd, uint32_t events)
{
	this->_einf.data.fd = fd;
	this->_einf.events = events;
	this->error_request = false;
	std::time(&this->_last_communication);
	return;
}

Client::~Client()
{
	return;
}

Client::Client(Client const& other)
{
	this->_einf = other._einf;
	this->_r = other._r;
	this->error_request = other.error_request;
	this->_last_communication = other._last_communication;
	return;
}

Client&	Client::operator=(Client const& other)
{
	if (this == &other)
		return *this;
	this->~Client();
	new (this) Client(other);
	return *this;
}

std::string const&	Client::getIp(void) const
{
	return this->_ip;
}

time_t			Client::getLastComm(void) const
{
	return this->_last_communication;
}

void	Client::setLastComm(void)
{
	std::time(&this->_last_communication);
}

std::string const&	Client::getPort(void) const
{
	return this->_port;
}

void			Client::setIpPort(std::string const& ip, std::string const& port)
{
	this->_ip = ip;
	this->_port = port;
}

int		Client::getFd(void) const
{
	return this->_einf.data.fd;
}

Request&		Client::getRequest(void)
{
	return this->_r;
}

std::string&	Client::getNotConstMsg(void)
{
	return this->_message;
}

std::string const&	Client::getMsg(void) const
{
	return this->_message;
}
