/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_request.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/07 19:49:10 by acamargo          #+#    #+#             */
/*   Updated: 2026/05/14 17:06:09 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cctype>
#include <cstddef>
#include <fstream>
#include <istream>
#include <map>
#include <sstream>
#include <string>
#include <utility>
#include "../includes/Request.hpp"

typedef struct s_token
{
	std::string	value;
}	t_token;

typedef bool(Request::*field_function)(std::string &);

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

void	parse_method(std::string &line, Request& r)
{
	size_t	pos = line.find_first_of(' ', 0);
	std::string	buff_tmp;

	while (pos != std::string::npos || !line.empty())
	{
		buff_tmp = line.substr(0, pos);
		if (!r.getMethod())
			r.setMethod(buff_tmp);
		else if (!r.getTargetResource())
			r.setTargetResource(buff_tmp);
		else if (!r.getProtocolV())
			r.setProtocolV(buff_tmp);
		else
			break;
		remove_spaces(line, pos);
		pos = line.find_first_of(' ', 0);
	}
	if (!r.getMethod() ||
		!r.getTargetResource() ||
		!r.getProtocolV())
		throw Request::BadRequest();
}

void	check_method(Request& r)
{
	if (r.getMethod()->compare(0, 4, "GET") &&
		r.getMethod()->compare(0, 5, "POST") &&
		r.getMethod()->compare(0, 7, "DELETE"))
		throw Request::BadRequest();
	if (r.getProtocolV()->compare(0, 8, "HTTP/1.1") &&
		r.getProtocolV()->compare(0, 9, "HTTP/1.1\r"))
		throw Request::BadRequest();
}

size_t	check_field_line_syntax(std::string const& line)
{
	size_t	colon_pos = line.find_first_of(':', 0);
	if (colon_pos == std::string::npos)
		throw Request::BadRequest();
	size_t	whitespace_pos = line.find_first_of(' ', 0);
	if (whitespace_pos < colon_pos)
		throw Request::BadRequest();
	return colon_pos;
}	

void	parse_line(std::string & line, Request& r, std::map<std::string, field_function> fields)
{
	if (!r.getMethod())
	{
		parse_method(line, r);
		check_method(r);
		return;
	}
	std::map<std::string, field_function>::iterator it;
	it = fields.find(line.substr(0, check_field_line_syntax(line)));
	if (it == fields.end())
		return;
	remove_whitespace(line);
	(r.*(it->second))(line);
}

void	parse_request(std::istringstream& request, Request &r)
{
	std::string	line;
	std::map<std::string, field_function> fields;
	std::pair<std::string, field_function>	field("Host", &Request::setHost);
	fields.insert(field);

	while (!request.eof())
	{
		std::getline(request, line);
		parse_line(line, r, fields);
	}
	throw Request::BadRequest();
	if (r.getHost().find("Host") == r.getHost().end())
		throw Request::BadRequest();
}
