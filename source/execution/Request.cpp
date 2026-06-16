/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/08 15:51:44 by acamargo          #+#    #+#             */
/*   Updated: 2026/06/16 13:48:30 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Header.hpp"
#include <Request.hpp>
#include <cstddef>
#include <limits>
#include <map>
#include <string>
#include <sys/types.h>

Request::Request()
{
	this->_request_in_progress = false;
	this->_body_len = 0;
	this->_bytes_read = 0;
	this->_loc_conf = NULL;
	return;
}

Request::~Request()
{
	return;
}

Request::Request(const Request& other)
{
	this->_header = other._header;
	this->_body = other._body;
	this->_response = other._response;
	this->_message = other._message;
	this->_request_in_progress = other._request_in_progress;
	this->_body_len = other._body_len;
	this->_bytes_read = other._bytes_read;
	this->_loc_conf = other._loc_conf;
	return ;
}

Request&	Request::operator=(const Request& other)
{
	if (&other == this)
		return *this;
	this->~Request();
	new (this) Request(other);
	return *this;
}

Header&		Request::getHeader(void)
{
	return this->_header;
}

const Header&		Request::getHeader(void) const
{
	return this->_header;
}

std::string const&			Request::getBody(void) const {
	return (this->_body);
}

void	Request::appendBody(std::string const& str)
{
	this->_body.append(str);
	return;
}

const char*	Request::ErrorRequest::what() const throw()
{
	return "Bad request.\n";
}

Request::ErrorRequest::ErrorRequest(int error_code)
{
	this->_error_code = error_code;
}

int		Request::ErrorRequest::getErrorCode(void) const
{
	return this->_error_code;
}

std::string const&		Request::getMessage(void) const
{
	return this->_message;
}

std::string&		Request::getNotConstMessage(void) 
{
	return this->_message;
}

void					Request::setMessage(std::string const& str)
{
	this->_message = str;
	return;
}

std::string const&		Request::getResponse(void) const
{
	return this->_response;
}

void					Request::setResponse(std::string const& str)
{
	this->_response = str;
	return;
}

bool					Request::getReqInProg(void) const
{
	return this->_request_in_progress;
}

void					Request::setReqInProg(bool value)
{
	this->_request_in_progress = value;
	return;
}

size_t	Request::getBytesRead(void) const
{
	return this->_bytes_read;
}

unsigned long long const&	Request::getBodyLen(void) const
{
	return this->_body_len;
}

bool	Request::addBytesRead(size_t bytes)
{
	size_t	diff = std::numeric_limits<size_t>::max() - this->_bytes_read;
	if (bytes > diff)
		return false;
	this->_bytes_read += bytes;
	return true;
}

void	Request::setBodyLen(unsigned long long const& len)
{
	this->_body_len = len;
	return;
}

locationConfig const*	Request::getLocConf(void) const
{
	return this->_loc_conf;
}

void	Request::setLocConf(locationConfig const& loc_conf)
{
	this->_loc_conf = &loc_conf;
}
