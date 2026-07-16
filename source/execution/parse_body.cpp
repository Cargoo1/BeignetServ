/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_body.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/01 13:36:16 by acamargo          #+#    #+#             */
/*   Updated: 2026/07/16 19:06:23 by acamargo         ###   ########.fr       */
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
#include <stdexcept>
#include <utils_logs.hpp>
#include <string>
#include <utils.hpp>

bool	read_trailer_fields(Request& r, std::string& raw_body)
{
	try
	{
		if (parse_fields(raw_body, r) == false)
			return false;
	}
	catch(Request::ErrorRequest &e)
	{
		print_log(TEXT_RED, &e, e.what(), 1);
		return e.getErrorCode();
	}
	return true;
}

int	read_with_boundaries(Request& r, std::string& raw_body)
{
	size_t boundary_pos = 0;
	Request temp_r;

	if (!r.is_body_being_read)
	{
		boundary_pos = raw_body.find(r.getHeader().getBoundary());
		if (boundary_pos == raw_body.npos)
			return 1;
		raw_body.erase(0, boundary_pos + r.getHeader().getBoundary().length());
		r.is_body_being_read = true;
	}
	std::string	boundary = CRLF + r.getHeader().getBoundary();
	temp_r.getHeader().set_is_header_parsed(true);
	temp_r.getHeader().is_method_parsed = true;
	boundary_pos = raw_body.find(boundary);
	while (boundary_pos != raw_body.npos)
	{
		consume_until_crlf(raw_body);
		int	return_value = read_trailer_fields(temp_r, raw_body);
		if (return_value == 1)
			return 1;
		else if (return_value > 1)
			return return_value;
		boundary_pos = raw_body.find(boundary);
		if (temp_r.getHeader().data_values[0] == "file")
		{
			r.getRawBody().append(raw_body, 0, boundary_pos);
			r.getHeader().setFilename(temp_r.getHeader().data_values[1]);
		}
		else
			print_log(TEXT_BLUE, NULL, "\nClient Comment:\n@@@\n" +
							raw_body.substr(0, boundary_pos) +
							"@@@\n", 0);
		raw_body.erase(0, boundary_pos + boundary.length());
		if (raw_body.find("--") == 0)
		{
			consume_until_crlf(raw_body);
			return 0;
		}
		boundary_pos = raw_body.find(boundary);
	}
	return 1;
}

bool	read_body(Request& r, std::string& raw_body)
{
	std::string	temp_str;
	std::map<std::string, std::string>::const_iterator	it = r.getHeader().getFields().find("Content-Type");

	if (!r.bytes_2_read)
		return true;
	if (r.getBytesRead() + r.bytes_2_read > r.getLocConfBlock()->getCMBS())
		throw Request::ErrorRequest(content_too_large, "Body max size reached!");
	if (it->second.find("multipart/") != std::string::npos)
		return read_with_boundaries(r, raw_body);
	temp_str = raw_body.substr(0, r.bytes_2_read);
	r.getRawBody().append(temp_str, 0, temp_str.length());
	r.addBytesRead(temp_str.length());
	raw_body.erase(0, temp_str.length());
	if (r.bytes_2_read != 0)
		return false;
	return true;
}

bool	read_chunked_body(Request& r, std::string& raw_body)
{
	size_t crlf_pos;
	crlf_pos = raw_body.find(CRLF, 0, CRLF_LEN);
	std::string	chunk_size;
	while (crlf_pos != raw_body.npos || r.waiting_chunk)
	{
		if (!r.waiting_chunk)
		{
			chunk_size = raw_body.substr(0, crlf_pos);
			print_log(TEXT_BLUE, NULL, "CHUNKED SIZE READ: " + chunk_size + '\n', false);
			consume_until_crlf(raw_body);
			r.bytes_2_read = hex_to_int(chunk_size);
			if (r.bytes_2_read == 0)
			{
				r.is_body_read = true;
				return true;
			}
		}
		if (read_body(r, raw_body) == false)
		{
			r.waiting_chunk = true;
			return false;
		}
		print_log(TEXT_BLUE, NULL, "DONE READING: " + raw_body + "<-\n", false);
		if (raw_body.find(CRLF) == raw_body.npos)
			return false;
		raw_body.erase(0, CRLF_LEN);
		r.waiting_chunk = false;
		crlf_pos = raw_body.find(CRLF, 0, CRLF_LEN);
	}
	return false;
}

bool	parse_body(Request& r, std::string& raw_body)
{
	std::map<std::string, std::string> const&	fields = r.getHeader().getFields();

	if (fields.find("Transfer-Encoding") != fields.end())
	{
		if (!r.is_body_read && read_chunked_body(r, raw_body) == false)
			return false;
		if (read_trailer_fields(r, raw_body) == false)
			return false;
	}
	else if (fields.find("Content-Length") != fields.end())
	{
		r.bytes_2_read = ft_atoull(fields.at("Content-Length").c_str());
		if (read_body(r, raw_body) == 1)
			return false;
	}
	r.setBodyLen(r.getBytesRead());
	return true;
}



