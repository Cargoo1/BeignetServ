/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_body.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/01 13:36:16 by acamargo          #+#    #+#             */
/*   Updated: 2026/07/09 21:30:19 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Request.hpp"
#include <cstddef>
#include <cstring>
#include <parse_request.hpp>
#include <iostream>
#include <sstream>
#include <utils_logs.hpp>
#include <string>
#include <utils.hpp>
int	read_body(Request& r)
{
	std::string	line;
	while (r.getBytesRead() < r.getBodyLen() && std::getline(r.getRawBody(), line))
	{
		remove_cr(line);
		r.appendBody(line);
		if (!r.addBytesRead(line.length()))
		{
			print_log(TEXT_RED, NULL, "Max body size reached", true);
			return 0;
		}
	}
	if (r.getBytesRead() == r.getBodyLen())
		return 0;
	return 1;
}

int	read_trailer_fields(Request& r, std::string& raw_body)
{
	try
	{
		if (parse_header(raw_body, r) == 1)
			return 1;
	}
	catch(Request::ErrorRequest &e)
	{
		print_log(TEXT_RED, &e, e.what(), 1);
		return e.getErrorCode();
	}
	return 0;
}

bool	read_chunked_body(Request& r, std::string& raw_body, std::stringstream& body_s)
{
	size_t crlf_pos;
	crlf_pos = raw_body.find(CRLF, 0, 2);
	std::string	line;
	while (crlf_pos != raw_body.npos)
	{
		if (!std::getline(body_s, line))
			return false;
		remove_cr(line);
		consume_until_crlf(raw_body);
		if (!r.waiting_chunk())
		{
			r.setBodyLen(hex_to_int(line));
			if (r.getBodyLen() == 0)
			{
				r.set_is_body_read(true);
				return true;
			}
		}
		if (!read_body(r))
		{
			r.setWaitingChunk(true);
			return false;
		}
		r.setWaitingChunk(false);
		raw_body.erase(0, crlf_pos + std::strlen(CRLF));
		crlf_pos = raw_body.find(CRLF, 0, 2);
	}
	return false;
}

int	parse_body(Request& r, std::string& raw_body)
{
	std::map<std::string, std::string> const&	fields = r.getHeader().getFields();
	std::stringstream	body_s(raw_body);
	if (fields.find("Transfer-Encoding") != fields.end())
	{
		if (!r.is_body_read() && !read_chunked_body(r, raw_body, body_s))
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
		if (read_body(r) == 1)
			return 1;
	}
	r.setBodyLen(r.getBody().length());
	return 0;
}

