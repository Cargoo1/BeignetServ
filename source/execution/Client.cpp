/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/14 19:50:59 by acamargo          #+#    #+#             */
/*   Updated: 2026/06/03 21:11:31 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Client.hpp>
#include <Request.hpp>
#include <iostream>

Client::Client(int fd, uint32_t events)
{
	this->_einf.data.fd = fd;
	this->_einf.events = events;
	std::time(&this->_last_communication);
	std::cout << fd << " " << this->_last_communication;
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
	this->_message = other._message;
	this->_response = other._response;
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

std::string const&	Client::getMessage(void) const
{
	return this->_message;
}

std::string const&	Client::getResponse(void) const
{
	return this->_response;
}

void			Client::setMessage(std::string const& msg)
{
	this->_message = msg;
}

void			Client::appendMessage(std::string const& s)
{
	this->_message += s;
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

void			Client::setResponse(std::string& response)
{
	this->_response = response;
}

int		Client::getFd(void) const
{
	return this->_einf.data.fd;
}

Request&		Client::getRequest(void)
{
	return this->_r;
}
