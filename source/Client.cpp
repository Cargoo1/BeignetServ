/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 15:15:33 by acamargo          #+#    #+#             */
/*   Updated: 2026/05/12 15:31:46 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Client.hpp"
#include <sys/poll.h>

Client::Client(void)
{
	this->_pfd.fd = 0;
	this->_pfd.events = 0;
	this->_pfd.revents = 0;
	return ;
}

Client::Client(Client const& other)
{
	this->_r = other._r;
	this->_message = other._message;
	this->_pfd.events = other._pfd.events;
	this->_pfd.fd = other._pfd.fd;
	this->_pfd.revents = other._pfd.revents;
	return;
}

Client::Client(int sfd, short events)
{
	this->_pfd.fd = sfd;
	this->_pfd.events = events;
	this->_pfd.revents = 0;
	return ;
}

Client::~Client(void)
{
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

struct pollfd&	Client::getPfd(void)
{
	return this->_pfd;
}

std::string&	Client::getMessage(void)
{
	return this->_message;
}

Request&	Client::getRequest(void)
{
	return this->_r;
}

void	Client::setRequest(Request const& r)
{
	this->_r = r;
	return;
}

void	Client::setMessage(std::string const& msg)
{
	this->_message = msg;
	return;
}

void	Client::setPfd(struct pollfd const& pfd)
{
	this->_pfd.events = pfd.events;
	this->_pfd.fd = pfd.fd;
	this->_pfd.revents = pfd.revents;
	return ;
}
