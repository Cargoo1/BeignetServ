/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/08 15:51:44 by acamargo          #+#    #+#             */
/*   Updated: 2026/05/14 17:59:16 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Request.hpp>
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

bool	Request::setMethod(std::string& method)
{
	if (this->_method)
		return false;
	this->_method = new std::string(method);
	return true;
}

bool	Request::setTargetResource(std::string& uri)
{
	if (this->_target_resource)
		return false;
	this->_target_resource = new std::string(uri);
	return true;
}

bool	Request::setProtocolV(std::string& protocol)
{
	if (this->_protocol_v)
		return false;
	this->_protocol_v = new std::string(protocol);
	return true;
}

bool	Request::setHost(std::string& host)
{
	size_t	pos;

	pos = host.find_first_of(':');
	if (pos == host.length() - 1)
		return false;
	host.erase(0, pos + 1);
	this->_host["Host"] = host;
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
