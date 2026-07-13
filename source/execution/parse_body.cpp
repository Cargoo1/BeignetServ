/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_body.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/01 13:36:16 by acamargo          #+#    #+#             */
/*   Updated: 2026/07/13 23:34:40 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parse_body.hpp"
#include "Client.hpp"
#include "Request.hpp"
#include <cstddef>
#include <cstring>
#include <map>
#include <parse_request.hpp>
#include <iostream>
#include <sstream>
#include <utils_logs.hpp>
#include <string>
#include <utils.hpp>

int	read_trailer_fields(Request& r, std::string& raw_body)
{
	try
	{
		if (parse_fields(raw_body, r) == 1)
			return 1;
	}
	catch(Request::ErrorRequest &e)
	{
		print_log(TEXT_RED, &e, e.what(), 1);
		return e.getErrorCode();
	}
	return 0;
}

int	read_with_boundaries(Request& r, std::string& raw_body)
{
	size_t boundary_pos = 0;

	if (!r.is_body_being_read)
	{
		boundary_pos = raw_body.find(r.getHeader().getBoundary());
		if (boundary_pos == raw_body.npos)
			return 1;
		raw_body.erase(0, boundary_pos + r.getHeader().getBoundary().length());
		r.is_body_being_read = true;
	}
	std::string	boundary = CRLF + r.getHeader().getBoundary();
	boundary_pos = raw_body.find(boundary);
	while (boundary_pos != raw_body.npos)
	{
		if (raw_body.find("--") == 0)
		{
			consume_until_crlf(raw_body);
			return 0;
		}
		consume_until_crlf(raw_body);
		int	return_value = read_trailer_fields(r, raw_body);
		if (return_value == 1)
			return 1;
		else if (return_value > 1)
			return return_value;
		boundary_pos = raw_body.find(boundary);
		if (r.getHeader().data_values[0] == "file")
			r.getRawBody().append(raw_body, 0, boundary_pos);
		else
			print_log(TEXT_BLUE, NULL, "\nClient Comment:\n@@@\n" +
							raw_body.substr(0, boundary_pos) +
							"@@@\n", 0);
		raw_body.erase(0, boundary_pos + boundary.length());
	}
	return 1;
}

int	read_body(Request& r, std::string& raw_body, size_t bytes_2_read)
{
	std::string	line;
	std::map<std::string, std::string>::const_iterator	it = r.getHeader().getFields().find("Content-Type");

	if (it->second.find("multipart/") != std::string::npos)
		return read_with_boundaries(r, raw_body);
	if (raw_body.length() < bytes_2_read)
		return 1;
	r.getRawBody().append(raw_body, 0, bytes_2_read);
	raw_body.erase(0, bytes_2_read);
	return 0;
}

bool	read_chunked_body(Request& r, std::string& raw_body)
{
	size_t crlf_pos;
	crlf_pos = raw_body.find(CRLF, 0, CRLF_LEN);
	std::string	line;
	size_t	chunked_len = 0;
	std::string	chunk_size;
	while (crlf_pos != raw_body.npos)
	{
		line = raw_body.substr(0, crlf_pos);
		consume_until_crlf(raw_body);
		if (!r.waiting_chunk)
		{
			chunked_len = hex_to_int(chunk_size);
			if (chunked_len == 0)
			{
				r.is_body_read = true;
				return true;
			}
		}
		if (raw_body.find(CRLF, CRLF_LEN) == raw_body.npos
			|| read_body(r, raw_body, chunked_len) == 1)
		{
			r.waiting_chunk = true;
			return false;
		}
		raw_body.erase(0, CRLF_LEN);
		r.waiting_chunk = false;
		crlf_pos = raw_body.find(CRLF, 0, CRLF_LEN);
	}
	return false;
}

int	parse_body(Request& r, std::string& raw_body)
{
	std::map<std::string, std::string> const&	fields = r.getHeader().getFields();

	if (fields.find("Transfer-Encoding") != fields.end())
	{
		if (!r.is_body_read && !read_chunked_body(r, raw_body))
			return 1;
		int	return_value = read_trailer_fields(r, raw_body);
		if (return_value > 1)
			return return_value;
		else if (return_value == 1)
			return 1;
	}
	else if (fields.find("Content-Length") != fields.end())
	{
		r.setBodyLen(ft_atoull(fields.at("Content-Length").c_str()));
		if (read_body(r, raw_body, r.getBodyLen()) == 1)
			return 1;
	}
	r.getHeader().setFilename(r.getHeader().data_values[1]);
	r.setBodyLen(r.getBody().length());
	return 0;
}

