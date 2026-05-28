/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Header.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/19 19:03:36 by acamargo          #+#    #+#             */
/*   Updated: 2026/05/28 21:11:37 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include <Header.hpp>
#include <cctype>
#include <cerrno>
#include <cstddef>
#include <HttpResponse.hpp>
#include <iostream>

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
		throw Request::ErrorRequest(bad_request);
	if (method.compare(0, 4, "GET") &&
		method.compare(0, 5, "POST") &&
		method.compare(0, 7, "DELETE"))
		throw Request::ErrorRequest(bad_request);
	this->_method = method;
	return true;
}

void	Header::setTargetResource(std::string& uri)
{
	if (uri.empty())
		throw Request::ErrorRequest(bad_request);
	int	deepness = 0;
	size_t	i = 0;
	while (i < uri.length())
	{
		if (std::isalnum(uri.at(i)))
		{
			while (i < uri.length() && uri.at(i) != '/')
				i++;
			deepness++;
		}
		else if (uri.at(i) == '/')
		{
			i++;
			while (i < uri.length() && uri.at(i) == '/')
				uri.erase(i, 1);
		}
		else if (uri.at(i) == '.')
		{
			if ((i + 1 == uri.length()) || (uri.at(i + 1) == '/'))
			{
				i++;
				continue;
			}
			else if (uri.at(i + 1) == '.' && (i + 2 == uri.length() || uri.at(i + 2) == '/'))
			{
				deepness--;
				i += 2;
				continue;
			}
			while (i < uri.length() && uri.at(i) != '/' && uri.at(i) != '?')
				i++;
			deepness++;
		}
		else if (uri.at(i) == '?')
			break;
		else
			throw Request::ErrorRequest(bad_request);
	}
	if (deepness < 0)
		throw Request::ErrorRequest(bad_request);
	this->_target_resource = uri.substr(0, i);
	std::cout << "path parsed: " + this->_target_resource + '\n';
}

bool	Header::setProtocolV(std::string& protocol)
{
	if (protocol.empty())
		throw Request::ErrorRequest(bad_request);
	if (protocol.compare(0, 8, "HTTP/1.1") &&
		protocol.compare(0, 9, "HTTP/1.1\r"))
		throw Request::ErrorRequest(bad_request);
	this->_protocol_v= protocol;
	return true;
}

void	Header::setHost(std::string& host)
{
	if (host.empty())
		throw Request::ErrorRequest(bad_request);
	size_t	colon_pos = host.find_first_of(":");
	if (colon_pos == std::string::npos)
		throw Request::ErrorRequest(bad_request);
	host.erase(0, colon_pos + 1);
	this->getFields()["Host"] = host;
}

void	Header::setContent_len(std::string& content_len)
{
	if (content_len.empty())
		throw Request::ErrorRequest(bad_request);
	size_t	colon_pos = content_len.find_first_of(":");
	if (colon_pos == std::string::npos)
		throw Request::ErrorRequest(bad_request);
	content_len.erase(0, colon_pos + 1);
	this->_map_fields["Content-Length"] = content_len;
}

std::string	&Header::getContentLenght(void)
{
	return (this->_map_fields["Content-Length"]);
}

const std::string&	Header::getContentLenght(void) const
{
	return (this->_map_fields.at("Content-Length"));
}
