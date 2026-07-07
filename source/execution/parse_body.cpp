/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_body.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/01 13:36:16 by acamargo          #+#    #+#             */
/*   Updated: 2026/07/07 17:59:56 by acamargo         ###   ########.fr       */
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

int	read_trailer_fields(Request& r, std::string& client_msg)
{
	if (client_msg.find("\r\n\r\n") == client_msg.npos)
		return 1;
	try
	{
		parse_header(r.getRawBody(), r);
	}
	catch(Request::ErrorRequest &e)
	{
		print_log(TEXT_RED, &e, e.what(), 1);
		return e.getErrorCode();
	}
	return 0;
}

bool	read_chunked_body(Request& r, std::string& client_msg, std::string& line)
{
	size_t crlf_pos;
	crlf_pos = client_msg.find(CRLF, 0, 2);
	while (crlf_pos != client_msg.npos)
	{
		if (!std::getline(r.getRawBody(), line))
			return false;
		remove_cr(line);
		if (!r.waiting_chunk())
		{
			r.setBodyLen(hex_to_int(line));
			if (r.getBodyLen() == 0)
			{
				client_msg.erase(0, crlf_pos + std::strlen(CRLF));
				return true;
			}
		}
		if (!read_body(r))
		{
			r.setWaitingChunk(true);
			return false;
		}
		r.setWaitingChunk(false);
		client_msg.erase(0, crlf_pos + std::strlen(CRLF));
		crlf_pos = client_msg.find(CRLF, 0, 2);
	}
	return false;
}

int	parse_body(Request& r, std::string& client_msg)
{
	r.getRawBody() << client_msg;
	std::map<std::string, std::string> const&	fields = r.getHeader().getFields();
	std::string	line;
	if (fields.find("Transfer-Encoding") != fields.end())
	{
		if (!read_chunked_body(r, client_msg, line))
			return 1;
		int	return_value = read_trailer_fields(r, client_msg);
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

