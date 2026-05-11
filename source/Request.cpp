/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/08 15:51:44 by acamargo          #+#    #+#             */
/*   Updated: 2026/05/09 12:44:32 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Request.hpp"
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

Request&	Request::operator=(const Request& other)
{
	if (&other == this)
		return *this;
	this->~Request();
	new (this) Request(other);
	return *this;
}
