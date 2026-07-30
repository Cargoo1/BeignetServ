/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_request.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/07 19:49:10 by acamargo          #+#    #+#             */
/*   Updated: 2026/07/30 20:31:00 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Header.hpp"
#include "HttpResponse.hpp"
#include "run_server.hpp"
#include "utils_logs.hpp"
#include <cctype>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <istream>
#include <map>
#include <sstream>
#include <string>
#include <utility>
#include <Request.hpp>
#include <utils.hpp>
#include <vector>
#include <parse_request.hpp>

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

	for (;pos < line.length(); ++pos)
	{
		if (line.at(pos) == '\"')
		{
			++pos;
			while (pos < line.length() && line.at(pos) != '\"')
				++pos;
		}
		else if (std::isspace(line.at(pos)))
			line.erase(pos--, 1);
	}
}

void	parse_method(std::string &line, Header& header, Request& r)
{
	std::vector<std::string>	vector;
	split(vector, line, ' ');
	if (vector.empty() || vector.size() < 3 || vector.size() > 3)
		throw  Request::ErrorRequest(bad_request, "Invalid request");
	header.setMethod(vector.at(0));
	header.setTargetResource(vector.at(1), r);
	header.setProtocolV(vector.at(2));
	if (header.getMethod().empty() ||
		header.getTargetResource().empty() ||
		header.getProtocolV().empty())
		throw Request::ErrorRequest(bad_request, "Not a complete request");
}

size_t	check_field_line_syntax(std::string const& line)
{
	if (line.empty())
		throw Request::ErrorRequest(bad_request, "Empty field");
	size_t	colon_pos = line.find_first_of(':', 0);
	if (colon_pos == 0 || colon_pos == line.length() - 1 || colon_pos == std::string::npos)
		throw Request::ErrorRequest(bad_request, "Invalid field syntax");
	size_t	whitespace_pos = line.find_first_of(' ', 0);
	if (whitespace_pos < colon_pos)
		throw Request::ErrorRequest(bad_request, "Invalid field syntax");
	return colon_pos;
}	

void	parse_line(std::string & line,
					Header& header,
					std::map<std::string,field_function> fields,
					Request& r)
{
	if (!header.is_method_parsed)
	{
		parse_method(line, header, r);
		header.is_method_parsed = true;
		return;
	}
	size_t	colon_pos = check_field_line_syntax(line);
	std::string field_name = line.substr(0, colon_pos);
	line.erase(0, colon_pos + 1);
	remove_whitespace(line);
	std::map<std::string, field_function>::iterator it;
	it = fields.find(field_name);
	if (it == fields.end())
		return;
	(header.*(it->second))(line);
}

#define FIELDS_SIZE 7

void	init_map_fields(std::map<std::string, field_function>& map_fields)
{
	std::pair<std::string, field_function>	field;
	std::string		name_fields[FIELDS_SIZE] = {"Host",
												"Content-Length",
												"Transfer-Encoding",
												"Content-Type",
												"Content-Disposition",
												"Cookie",
												"Status"
											};
	field_function	fn_fields[FIELDS_SIZE] = {&Header::setHost,
												&Header::setContent_len,
												&Header::setTransfer_encoding,
												&Header::setContent_type,
												&Header::setContent_dispo,
												&Header::setCookie,
												&Header::setStatus};

	for (size_t i = 0; i < FIELDS_SIZE; i++)
	{
		field.first = name_fields[i];
		field.second = fn_fields[i];
		map_fields.insert(field);
	}
}

int	parse_fields(std::string& request,
						Request& r)
{
	std::string	line;
	Header&	header = r.getHeader();
	std::map<std::string, field_function> map_fields;
	size_t	crlf_pos = request.find(CRLF);

	init_map_fields(map_fields);
	while (crlf_pos != request.npos)
	{
		line = request.substr(0, crlf_pos);
		consume_until_crlf(request);
		if (line.empty())
			return DONE;
		parse_line(line, header, map_fields, r);
		crlf_pos = request.find(CRLF);
	}
	return INCOMPLETE;
}
