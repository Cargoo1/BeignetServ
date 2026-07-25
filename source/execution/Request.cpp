/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/08 15:51:44 by acamargo          #+#    #+#             */
/*   Updated: 2026/07/25 14:45:59 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Header.hpp"
#include "HttpResponse.hpp"
#include "utils.hpp"
#include <Request.hpp>
#include <cstddef>
#include <limits>
#include <map>
#include <string>
#include <sys/types.h>
#include <iostream>

Request::Request() : _header(*this)
{
	this->_request_in_progress = false;
	this->is_request_done = false;
	this->is_cgi_in_progress = false;
	this->_body_len = 0;
	this->_bytes_read = 0;
	this->_locConf_block = NULL;
	this->_server_block = NULL;
	this->is_body_read = false;
	this->waiting_chunk = false;
	this->is_body_being_read = false;
	this->_pipe_fd = -1;
	return;
}

Request::~Request()
{
	return;
}

Request::Request(const Request& other) : _header(*this)
{
	this->_header = other._header;
	this->is_request_done = other.is_request_done;
	this->_body = other._body;
	this->_response = other._response;
	this->_request_in_progress = other._request_in_progress;
	this->is_cgi_in_progress = other.is_cgi_in_progress;
	this->_body_len = other._body_len;
	this->_bytes_read = other._bytes_read;
	this->_locConf_block = other._locConf_block;
	this->_server_block = other._server_block;
	this->is_body_read = other.is_body_read;
	this->is_body_being_read = other.is_body_being_read;
	this->waiting_chunk = other.waiting_chunk;
	this->data_name = other.data_name;
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
	return this->reason;
}

Request::ErrorRequest::ErrorRequest(int error_code, char const* reason)
{
	this->_error_code = error_code;
	this->reason = reason;

}

int		Request::ErrorRequest::getErrorCode(void) const
{
	return this->_error_code;
}

HttpResponse&		Request::getResponse(void)
{
	return this->_response;
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
	if (bytes > this->bytes_2_read)
		this->bytes_2_read = 0;
	else
		this->bytes_2_read -= bytes;
	return true;
}

bool	Request::addBodyLen(size_t bytes)
{
	size_t	diff = std::numeric_limits<unsigned long long>::max() - this->_bytes_read;
	if (bytes > diff)
		return false;
	this->_body_len += bytes;
	return true;
}

void	Request::setBodyLen(unsigned long long const& len)
{
	this->_body_len = len;
	return;
}

locationConfig const*	Request::getLocConfBlock(void) const
{
	return this->_locConf_block;
}

void	Request::setLocConfBlock(locationConfig const& loc_conf)
{
	this->_locConf_block = &loc_conf;
}

serverConfig const*	Request::getServerBlock(void) const
{
	return this->_server_block;

}

int		Request::setServerBlock(serverConfig const& server_block)
{
	this->_server_block = &server_block;
	return 0;
}

std::string&	Request::getRawBody(void)
{
	return this->_body;
}

std::stringstream&	Request::getRequestStream(void)
{
	return this->_request_stream;
}

void	Request::setBoundary(std::string const& str)
{
	this->_boundary = str;
}

std::string&	Request::getBoundary(void)
{
	return this->_boundary;
}

bool	Request::setPipeFd(int pipe_fd)
{
	if (pipe_fd < 0)
		return false;
	this->_pipe_fd = pipe_fd;
	return true;
}

int		Request::getPipeFd(void) const
{
	return this->_pipe_fd;
}

std::string&	Request::getScripOutput(void)
{
	return this->_script_output;
}
