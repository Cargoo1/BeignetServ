/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Header.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/19 19:03:36 by acamargo          #+#    #+#             */
/*   Updated: 2026/05/19 21:12:45 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include <Header.hpp>
#include <cstddef>

Header::Header()
{
	return;
}

Header::Header(Header const& other)
{
	this->_map_fields = other._map_fields;
	this->_method = other._method;
	this->_protocol_v = other._protocol_v;
	this->_target_resource = other._target_resource;
	return;
}

Header&		Header::operator=(Header const& other)
{
	if (this == &other)
		return *this;
	this->~Header();
	new (this) Header(other);
	return *this;
}

Header::~Header()
{
	return;
}

std::string&	Header::getMethod(void)
{
	return this->_method;
}

std::string&	Header::getTargetResource(void)
{
	return this->_target_resource;
}

const std::string&	Header::getTargetResource(void) const
{
	return this->_target_resource;
}

std::string&	Header::getProtocolV(void)
{
	return this->_protocol_v;
}

std::map<std::string, std::string>&		Header::getFields(void)
{
	return this->_map_fields;
}

bool	Header::setMethod(std::string& method)
{
	if (method.empty())
		throw Request::BadRequest();
	if (method.compare(0, 4, "GET") &&
		method.compare(0, 5, "POST") &&
		method.compare(0, 7, "DELETE"))
		throw Request::BadRequest();
	this->_method = method;
	return true;
}

bool	Header::setTargetResource(std::string& uri)
{
	if (uri.empty())
		throw Request::BadRequest();
	this->_target_resource = uri;
	return true;
}

bool	Header::setProtocolV(std::string& protocol)
{
	if (protocol.empty())
		throw Request::BadRequest();
	if (protocol.compare(0, 8, "HTTP/1.1") &&
		protocol.compare(0, 9, "HTTP/1.1\r"))
		throw Request::BadRequest();
	this->_protocol_v= protocol;
	return true;
}

void	Header::setHost(std::string& host)
{
	if (host.empty())
		throw Request::BadRequest();
	size_t	colon_pos = host.find_first_of(":");
	if (colon_pos == std::string::npos)
		throw Request::BadRequest();
	host.erase(0, colon_pos + 1);
	this->getFields()["Host"] = host;
}

void	Header::setContent_len(std::string& content_len)
{
	if (content_len.empty())
		throw Request::BadRequest();
	size_t	colon_pos = content_len.find_first_of(":");
	if (colon_pos == std::string::npos)
		throw Request::BadRequest();
	content_len.erase(0, colon_pos + 1);
	this->_map_fields["Content-Length"] = content_len;
}

std::string	Header::getContentLenght(void)
{
	return (this->_map_fields["Content-Length"]);
}

const std::string	Header::getContentLenght(void) const
{
	return (this->_map_fields["Content-Length"]);
}