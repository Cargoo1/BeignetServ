/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_request.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/07 19:49:10 by acamargo          #+#    #+#             */
/*   Updated: 2026/05/27 14:16:35 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Header.hpp"
#include "HttpResponse.hpp"
#include <cctype>
#include <cstddef>
#include <fstream>
#include <istream>
#include <map>
#include <sstream>
#include <string>
#include <utility>
#include <Request.hpp>

typedef void(Header::*field_function)(std::string &);

void	remove_spaces(std::string& line, size_t pos)
{
	size_t	pos_2;

	line.erase(0, pos);
	pos_2 = line.find_first_not_of(' ', 0);
	line.erase(0, pos_2);
}

void	remove_whitespace(std::string& line)
{
	size_t	pos = 0;

	while (pos < line.length())
	{
		if (std::isspace(line.at(pos)))
			line.erase(pos, 1);
		pos++;
	}
}

void	parse_method(std::string &line, Header& header)
{
	size_t	pos = line.find_first_of(' ', 0);
	std::string	buff_tmp;

	while (pos != std::string::npos || !line.empty())
	{
		buff_tmp = line.substr(0, pos);
		if (header.getMethod().empty())
			header.setMethod(buff_tmp);
		else if (header.getTargetResource().empty())
			header.setTargetResource(buff_tmp);
		else if (header.getProtocolV().empty())
			header.setProtocolV(buff_tmp);
		else
			break;
		remove_spaces(line, pos);
		pos = line.find_first_of(' ', 0);
	}
	if (header.getMethod().empty() ||
		header.getTargetResource().empty() ||
		header.getProtocolV().empty())
		throw Request::ErrorRequest(400);
}

size_t	check_field_line_syntax(std::string const& line)
{
	size_t	colon_pos = line.find_first_of(':', 0);
	if (colon_pos == std::string::npos)
		throw Request::ErrorRequest(bad_request);
	size_t	whitespace_pos = line.find_first_of(' ', 0);
	if (whitespace_pos < colon_pos)
		throw Request::ErrorRequest(400);
	return colon_pos;
}	

void	parse_line(std::string & line, Header& header, std::map<std::string, field_function> fields)
{
	if (header.getMethod().empty())
	{
		parse_method(line, header);
		return;
	}
	remove_whitespace(line);
	if (line.empty())
		return;
	std::map<std::string, field_function>::iterator it;
	it = fields.find(line.substr(0, check_field_line_syntax(line)));
	if (it == fields.end())
		return;
	remove_whitespace(line);
	(header.*(it->second))(line);
}

#define FIELDS_SIZE 2

void	init_map_fields(std::map<std::string, field_function>& map_fields)
{
	std::pair<std::string, field_function>	field;
	std::string		name_fields[FIELDS_SIZE] = {"Host",
												"Content-Length"};
	field_function	fn_fields[FIELDS_SIZE] = {&Header::setHost,
												&Header::setContent_len};

	for (size_t i = 0; i < FIELDS_SIZE; i++)
	{
		field.first = name_fields[i];
		field.second = fn_fields[i];
		map_fields.insert(field);
	}
}

void	parse_header(std::istringstream& request, Header &header)
{
	std::string	line;
	std::map<std::string, field_function> map_fields;

	init_map_fields(map_fields);
	while (std::getline(request, line) && line.compare(0, 2, "\r"))
	{
		parse_line(line, header, map_fields);
	}
	if (header.getFields().find("Host") == header.getFields().end())
		throw Request::ErrorRequest(bad_request);
}
