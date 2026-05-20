/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/08 15:51:44 by acamargo          #+#    #+#             */
/*   Updated: 2026/05/19 19:34:31 by acamargo         ###   ########.fr       */
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

const char*	Request::BadRequest::what() const throw()
{
	return "Bad request.\n";
}
