/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Header.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ratel <ratel@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/19 19:03:36 by acamargo          #+#    #+#             */
/*   Updated: 2026/07/14 16:04:51 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include <Request.hpp>
#include <Header.hpp>
#include <cctype>
#include <cerrno>
#include <cstddef>
#include <HttpResponse.hpp>
#include <iostream>
#include <utils.hpp>

Header::Header() : is_method_parsed(false),
					_is_header_parsed(false),
					_is_script(false)
{
	return;
}

Header::Header(Header const& other)
{
	this->_map_fields = other._map_fields;
	this->_method = other._method;
	this->_protocol_v = other._protocol_v;
	this->_target_resource = other._target_resource;
	this->_is_header_parsed = other._is_header_parsed;
	this->is_method_parsed = other.is_method_parsed;
	this->_is_script = other._is_script;
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

const std::string&	Header::getMethod(void) const
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

const std::string&	Header::getProtocolV(void) const
{
	return this->_protocol_v;
}

std::map<std::string, std::string>&		Header::getFields(void)
{
	return this->_map_fields;
}

const std::map<std::string, std::string>&		Header::getFields(void) const
{
	return this->_map_fields;
}

std::string&	Header::getBoundary(void)
{
	return this->_boundary;
}

bool	Header::setMethod(std::string& method)
{
	if (method.empty())
		throw Request::ErrorRequest(bad_request, "No method to parse");
	if (method.compare(0, 3, "GET") &&
		method.compare(0, 4, "POST") &&
		method.compare(0, 6, "DELETE"))
		throw Request::ErrorRequest(method_not_allowed, "method_not_allowed");
	this->_method = method;
	return true;
}

namespace { size_t	parse_path(std::string& uri)
{
	int	deepness = 0;
	size_t	i = 0;
	while (i < uri.length() && deepness >= 0)
	{
		if (is_unreserved(uri.at(i)))
		{
			if (uri.at(i) == '.')
			{
				if ((i + 1 == uri.length()) || !is_unreserved(uri.at(i + 1)))
				{
					i++;
					continue;
				}
				else if (uri.at(i + 1) == '.'
						&& (i + 2 == uri.length() || !is_unreserved(uri.at(i + 2))))
				{
					deepness--;
					i += 2;
					continue;
				}
			}
			while (i < uri.length()
				&& is_unreserved(uri.at(i)))
				i++;
			deepness++;
		}
		else if (uri.at(i) == '/')
		{
			i++;
			while (i < uri.length() && uri.at(i) == '/')
				uri.erase(i, 1);
		}
		else if (uri.at(i) == '?')
			break;
		else if (uri.at(i) == '%' && i + 1 < uri.length())
		{
			if (!decode_percent_encoding(uri, i))
				throw Request::ErrorRequest(bad_request, "Could not decode a PE char");
		}
		else
			throw Request::ErrorRequest(bad_request, "Found a not valid char");
	}
	if (deepness < 0)
		throw Request::ErrorRequest(bad_request, "Invalid path");
	if (i >= uri.length())
		return std::string::npos;
	return i;
	}
}

void	Header::setTargetResource(std::string& uri, Request& r)
{
	if (uri.empty())
		throw Request::ErrorRequest(bad_request, "Empty URI");
	std::string	prefix = ".";
	std::string	root;
	size_t separator_pos = parse_path(uri);
	r.setLocConfBlock(find_location_block(uri, *r.getServerBlock()));
	this->_target_resource = uri.substr(0, separator_pos);
	if (r.getLocConfBlock()->getRoot().empty())
		root = r.getServerBlock()->getRoot();
	else
		root = r.getLocConfBlock()->getRoot();
	if (root.at(0) != '/')
		prefix += '/';
	this->_target_resource = prefix + root + this->_target_resource;
	if (find_type(this->_target_resource) == CGI_PY || is_in_cgi_dir(uri))
		this->_is_script = true;
	if (separator_pos == std::string::npos)
		return;
	if (separator_pos < uri.length())
		separator_pos++;
	this->_query_string = uri.substr(separator_pos, std::string::npos);
}

bool	Header::setProtocolV(std::string& protocol)
{
	if (protocol.empty())
		throw Request::ErrorRequest(bad_request, "Empty protocol");
	if (protocol.compare(0, 8, "HTTP/1.1") &&
		protocol.compare(0, 9, "HTTP/1.1\r"))
		throw Request::ErrorRequest(bad_request, "Request function has a different http version");
	this->_protocol_v= protocol;
	return true;
}

void	Header::setHost(std::string& host)
{
	if (host.empty())
		throw Request::ErrorRequest(bad_request, "Empty host");
	size_t	colon_pos = host.find_first_of(":");
	if (colon_pos == std::string::npos)
		throw Request::ErrorRequest(bad_request, "Invalid host");
	host.erase(0, colon_pos + 1);
	this->getFields()["Host"] = host;
}

void	Header::setContent_len(std::string& content_len)
{
	if (content_len.empty())
		throw Request::ErrorRequest(bad_request, "Invalid Content-Length");
	size_t	colon_pos = content_len.find_first_of(":");
	if (colon_pos == std::string::npos)
		throw Request::ErrorRequest(bad_request, "Invalid Content-Length");
	content_len.erase(0, colon_pos + 1);
	for (size_t i = 0; i < content_len.length(); ++i)
	{
		if (!std::isdigit(content_len.at(i)))
			throw Request::ErrorRequest(bad_request, "Invalid Content-Length");
	}
	this->_map_fields["Content-Length"] = content_len;
}

void	Header::setContent_type(std::string& content_type)
{
	size_t	separator_pos = content_type.find_first_of(':',0);

	if (separator_pos == content_type.npos)
		throw Request::ErrorRequest(bad_request, "Incomplete field");
	content_type.erase(0, separator_pos + 1);
	this->_map_fields["Content-Type"] = content_type;
	separator_pos = content_type.find_first_of(';', 0);
	if (content_type.substr(0, separator_pos).find("multipart/") == content_type.npos)
	{
		this->_map_fields["Content-Type"] = content_type;
		return;
	}
	if (separator_pos == content_type.npos || separator_pos == content_type.length() - 1)
		throw Request::ErrorRequest(bad_request, "Parameter required in Content-type");
	separator_pos = content_type.find_first_of('=');
	if (separator_pos == content_type.npos)
		throw Request::ErrorRequest(bad_request, "Syntax error");
	content_type.erase(0, separator_pos + 1);
	std::string	boundary = content_type;
	if (boundary.length() > 70)
		throw Request::ErrorRequest(bad_request, "Boundary too long");
	if (boundary.at(0) != '"' || boundary.at(boundary.length() - 1) != '"')
	{
		for (size_t i = 0; i < boundary.length(); i++)
			if (!is_unreserved(boundary.at(i)))
				throw Request::ErrorRequest(bad_request, "Reserved char found");
	}
	if (boundary.at(0) == '"' && boundary.at(boundary.length() - 1) == '"')
	{
		boundary.erase(0, 1);
		boundary.erase(boundary.length() - 1, 1);
	}
	this->_boundary = "--"  + boundary;
}

void	Header::setTransfer_encoding(std::string& transfer_encoding)
{
	if (transfer_encoding.empty())
		throw Request::ErrorRequest(bad_request, "Incomplete transfer_encoding");
	size_t	colon_pos = transfer_encoding.find_first_of(":");
	if (colon_pos == std::string::npos)
		throw Request::ErrorRequest(bad_request, "Invalid transfer_encoding");
	transfer_encoding.erase(0, colon_pos + 1);
	if (transfer_encoding == "gzip" || transfer_encoding == "chunked")
	{
		this->_map_fields["Transfer-Encoding"] = transfer_encoding;
		return;
	}
	throw Request::ErrorRequest(bad_request, "Invalid transfer_encoding");
}

void	Header::setContent_dispo(std::string& content_type)
{
	size_t	separator_pos = content_type.find(':');
	if (separator_pos == content_type.npos)
		throw Request::ErrorRequest(bad_request, "Incomplete field");
	content_type.erase(0, separator_pos + 1);
	this->_map_fields["Content-Disposition"] = content_type;
	std::string	data_variables[2] = {"name", "filename"};
	size_t begin_pos = 0;
	size_t end_pos = 0;
	for (size_t i = 0; i < 2; i++)
	{
		begin_pos = content_type.find(data_variables[i] + "=\"");
		if (begin_pos == content_type.npos)
			continue;
		begin_pos += data_variables[i].length() + 2;
		end_pos = content_type.find("\"", begin_pos);
		if (end_pos == content_type.npos)
			continue;
		this->data_values[i] = content_type.substr(begin_pos, end_pos - begin_pos);
	}
}

std::string	&Header::getContentLenght(void)
{
	return (this->_map_fields["Content-Length"]);
}

const std::string&	Header::getContentLenght(void) const
{
	return (this->_map_fields.at("Content-Length"));
}

bool	Header::is_header_parsed(void) const
{
	return this->_is_header_parsed;
}

void	Header::set_is_header_parsed(bool value)
{
	this->_is_header_parsed = value;
}

bool	Header::is_a_script(void) const
{
	return this->_is_script;
}

std::string const& Header::getFilename(void) const
{
	return this->_file_name;
}

void	Header::setFilename(std::string const& str)
{
	this->_file_name = str;
}
