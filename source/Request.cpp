/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/08 15:51:44 by acamargo          #+#    #+#             */
/*   Updated: 2026/05/12 23:13:23 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Request.hpp"
#include <cstddef>
#include <map>
#include <string>

Request::Request(std::string& method, std::string& uri, std::string& protocol_v,
				std::string& host, std::string& port)
{
	this->_method = new std::string(method);
	this->_target_resource = new std::string(uri);
	this->_protocol_v = new std::string(protocol_v);
	this->_host["host"] = host;
	this->_host["port"] = port;
	return;
}

Request::Request() : _method(NULL), _target_resource(NULL), _protocol_v(NULL)
{
	return ;
}

Request::~Request()
{
	if (this->_method)
		delete this->_method;
	if (this->_target_resource)
		delete this->_target_resource;
	if (this->_protocol_v)
		delete this->_protocol_v;
	return;
}

Request::Request(const Request& other): _method(other._method),
										_target_resource(other._target_resource),
										_protocol_v(other._protocol_v),
										_host(other._host)
{
	return ;
}

std::string const*	Request::getMethod(void) const
{
	return this->_method;
}

std::string const*	Request::getTargetResource(void) const
{
	return this->_target_resource;
}

std::string const*	Request::getProtocolV(void) const
{
	return this->_protocol_v;
}

std::map<std::string const, std::string> const&	Request::getHost(void) const
{
	return this->_host;
}

bool	Request::setMethod(std::string const& method)
{
	if (this->_method)
		return false;
	this->_method = new std::string(method);
	return true;
}

bool	Request::setTargetResource(std::string const& uri)
{
	if (this->_target_resource)
		return false;
	this->_target_resource = new std::string(uri);
	return true;
}

bool	Request::setProtocolV(std::string const& protocol)
{
	if (this->_protocol_v)
		return false;
	this->_protocol_v = new std::string(protocol);
	return true;
}

bool	Request::setHost(std::string const& host)
{
	size_t	pos;
	if (!this->_host.empty())
		return false;
	pos = host.find_first_of(':');
	if (pos == std::string::npos)
	{
		this->_host["host"] = host;
		return true;
	}
	this->_host["host"] = host.substr(0, pos);
	this->_host["port"] = host.substr(pos, std::string::npos);
	return true;
}

Request&	Request::operator=(const Request& other)
{
	if (&other == this)
		return *this;
	this->~Request();
	new (this) Request(other);
	return *this;
}

const char*	Request::BadRequest::what() const throw()
{
	return "Bad request.\n";
}
