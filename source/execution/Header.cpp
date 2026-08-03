/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Header.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ratel <ratel@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/19 19:03:36 by acamargo          #+#    #+#             */
/*   Updated: 2026/08/03 20:05:52 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include <utils_logs.hpp>
#include <Request.hpp>
#include <Header.hpp>
#include <cctype>
#include <cstddef>
#include <HttpResponse.hpp>
#include <utils.hpp>

Header::Header(Request const& r) : is_method_parsed(false),
					_is_header_parsed(false),
					_request(r),
					_is_a_script(false)
{
	Request const& temp = this->_request;
	(void)temp;
	return;
}

Header::Header(Header const& other) : _request(other._request)
{
	this->_map_fields = other._map_fields;
	this->_method = other._method;
	this->_protocol_v = other._protocol_v;
	this->_target_resource = other._target_resource;
	this->_is_header_parsed = other._is_header_parsed;
	this->is_method_parsed = other.is_method_parsed;
	this->_is_a_script = other._is_a_script;
	return;
}

Header&		Header::operator=(Header const& other)
{
	if (this == &other)
		return *this;
	this->_map_fields = other._map_fields;
	this->_method = other._method;
	this->_protocol_v = other._protocol_v;
	this->_target_resource = other._target_resource;
	this->_is_header_parsed = other._is_header_parsed;
	this->is_method_parsed = other.is_method_parsed;
	this->_is_a_script = other._is_a_script;
	return *this;
}

Header::~Header()
{
	return;
}

Request const&	Header::getRequest(void) const
{
	return this->_request;
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
		throw Request::ErrorRequest(bad_request, "Not a valid method");
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

static bool	set_root_path(Request const& r, std::string& real_path, std::string const& uri)
{
	std::string	prefix = ".";
	std::string	root;
	if (r.getHeader().getMethod() == "POST" && !r.getLocConfBlock()->getUploadStore().empty())
		root = r.getLocConfBlock()->getUploadStore();
	else if (!r.getLocConfBlock()->getRoot().empty())
		root = r.getLocConfBlock()->getRoot();
	else
		root = r.getServerBlock()->getRoot();
	if (root.at(0) != '/')
		prefix += '/';
	real_path = uri;
	size_t	loc_conf_path = real_path.find(r.getLocConfBlock()->getPath());
	if (loc_conf_path != real_path.npos && r.getLocConfBlock()->getPath().length() > 1)
	{
		if (real_path.at(loc_conf_path) == '/')
			++loc_conf_path;
		if (loc_conf_path < real_path.length())
			real_path.erase(loc_conf_path, r.getLocConfBlock()->getPath().length() - 1);
	}
	real_path.insert(0, prefix + root);
	print_log(TEXT_GREEN, NULL, 
	"Final URI:"
	+ real_path, false);
	return true;
}

void	Header::setTargetResource(std::string& uri, Request& r)
{
	if (uri.empty())
		throw Request::ErrorRequest(bad_request, "Empty URI");
	r.setLocConfBlock(find_location_block(uri, *r.getServerBlock()));
	set_root_path(r, this->_target_resource, uri);
	size_t	separator_pos = parse_path(this->_target_resource);
	if (separator_pos != std::string::npos
		&& separator_pos < this->_target_resource.length())
	{
		this->_query_string = this->_target_resource.substr(separator_pos + 1);
		this->_target_resource.erase(separator_pos, std::string::npos);
	}
	size_t	path_extra_pos = std::string::npos;
	if (is_a_cgi_request(this->_target_resource, path_extra_pos))
	{
		if (path_extra_pos != std::string::npos)
		{
			this->_path_info = this->_target_resource.substr(path_extra_pos);
			this->_target_resource = this->_target_resource.substr(0, path_extra_pos);
		}
		this->_is_a_script = true;
	}
}

bool	Header::setProtocolV(std::string& protocol)
{
	if (protocol.empty())
		throw Request::ErrorRequest(bad_request, "Empty protocol");
	if (protocol.compare(0, 8, "HTTP/1.1"))
		throw Request::ErrorRequest(http_version_not_supported, "Request function has a different http version");
	this->_protocol_v= protocol;
	return true;
}

void	Header::setHost(std::string& host)
{
	if (host.empty())
		throw Request::ErrorRequest(bad_request, "Empty host");
	size_t	colon_pos = host.find_first_of(":");
	if (colon_pos == host.length() - 1)
		throw Request::ErrorRequest(bad_request, "Invalid host");
	this->getFields()["Host"] = host;
}

void	Header::setContent_len(std::string& content_len)
{
	if (content_len.empty())
		throw Request::ErrorRequest(bad_request, "Invalid Content-Length");
	for (size_t i = 0; i < content_len.length(); ++i)
	{
		if (!std::isdigit(content_len.at(i)))
			throw Request::ErrorRequest(bad_request, "Invalid Content-Length");
	}
	if (ft_atoull(content_len.c_str()) > this->_request.getLocConfBlock()->getCMBS())
		throw Request::ErrorRequest(content_too_large, "Content-Length too large");
	this->_map_fields["Content-Length"] = content_len;
}

void	Header::setCookie(std::string& cookie)
{
	if (cookie.empty())
		throw Request::ErrorRequest(bad_request, "Invalid Cookie");
	this->_map_fields["Cookie"] = cookie;
}

void	Header::setContent_type(std::string& content_type)
{
	size_t	separator_pos = 0;

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
	if (transfer_encoding == "gzip" || transfer_encoding == "chunked")
	{
		this->_map_fields["Transfer-Encoding"] = transfer_encoding;
		return;
	}
	throw Request::ErrorRequest(bad_request, "Invalid transfer_encoding");
}

void	Header::setContent_dispo(std::string& content_type)
{
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

void	Header::setStatus(std::string& str)
{
	this->_map_fields["Status"] = str;
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
	return this->_is_a_script;
}

std::string const& Header::getFilename(void) const
{
	return this->_file_name;
}

void	Header::setFilename(std::string const& str)
{
	this->_file_name = str;
}

std::string const&	Header::getQueryStr(void) const
{
	return this->_query_string;
}

std::string const& Header::getPathInfo(void) const
{
	return this->_path_info;
}

std::string const*	Header::getContentType(void) const
{
	if (this->_map_fields.find("Content-Type") == this->_map_fields.end())
		return NULL;
	return &this->_map_fields.at("Content-Type");
}

std::string const&	Header::getCookie(void) const
{
	if (this->_map_fields.find("Cookie") == this->_map_fields.end())
	{
		static const std::string empty_cookie;
		return empty_cookie;
	}
	return this->_map_fields.at("Cookie");
}

std::string&	Header::getCookie(void)
{
	return this->_map_fields["Cookie"];
}
