/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/14 19:50:59 by acamargo          #+#    #+#             */
/*   Updated: 2026/05/14 20:36:39 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Client.hpp>
#include <Request.hpp>

Client::Client(int fd)
{
	this->fd = fd;
	return;
}

Client::~Client()
{
	return;
}

Client::Client(Client const& other)
{
	this->fd = other.fd;
	this->_r = other._r;
	this->message = other.message;
	this->response = other.response;
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

std::string&	Client::getMessage(void)
{
	return this->message;
}

std::string&	Client::getResponse(void)
{
	return this->response;
}

void			Client::setMessage(std::string& msg)
{
	this->message = msg;
}

void			Client::setResponse(std::string& response)
{
	this->response = response;
}
