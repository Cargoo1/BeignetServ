/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/08 15:51:44 by acamargo          #+#    #+#             */
/*   Updated: 2026/05/26 19:28:40 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Header.hpp"
#include <Request.hpp>
#include <cstddef>
#include <map>
#include <string>

Request::Request()
{
	return;
}

Request::~Request()
{
	return;
}

Request::Request(const Request& other)
{
	this->_header = other._header;
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

const std::string			Request::getBody(void) const {
	return (this->_body);
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
