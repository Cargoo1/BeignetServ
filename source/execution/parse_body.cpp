/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_body.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/01 13:36:16 by acamargo          #+#    #+#             */
/*   Updated: 2026/07/11 15:05:24 by acamargo         ###   ########.fr       */
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
int	read_body(Request& r, std::string& raw_body, size_t bytes_2_read)
{
	std::string	line;
	std::istringstream	body_s(raw_body);
	while (r.getBytesRead() < bytes_2_read && std::getline(body_s, line))
	{
		remove_cr(line);
		consume_until_crlf(raw_body);
		r.appendBody(line);
		if (!r.addBodyLen(line.length()) || !r.addBytesRead(line.length()))
		{
			print_log(TEXT_RED, NULL, "Max body size reached", true);
			return 0;
		}
	}
	if (r.getBytesRead() == bytes_2_read)
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
	size_t	chunked_len = 0;
	while (crlf_pos != raw_body.npos)
	{
		if (!std::getline(body_s, line))
			return false;
		remove_cr(line);
		consume_until_crlf(raw_body);
		if (!r.waiting_chunk())
		{
			chunked_len = hex_to_int(line);
			if (chunked_len == 0)
			{
				r.set_is_body_read(true);
				return true;
			}
		}
		if (read_body(r, raw_body, chunked_len) == 1)
		{
			r.setWaitingChunk(true);
			return false;
		}
		r.setWaitingChunk(false);
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
		if (read_body(r, raw_body, r.getBodyLen()) == 1)
			return 1;
	}
	r.setBodyLen(r.getBody().length());
	return 0;
}

