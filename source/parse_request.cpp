/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_request.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/07 19:49:10 by acamargo          #+#    #+#             */
/*   Updated: 2026/05/11 20:18:06 by alejandrocama    ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstddef>
#include <fstream>
#include <istream>
#include <map>
#include <sstream>
#include <string>
#include "../includes/Request.hpp"

typedef struct s_token
{
	std::string	value;
}	t_token;

void	parse_method(std::string &line, Request& r)
{
	size_t	pos = line.find_first_of(" ", 0);
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
		if (pos == std::string::npos)
			line.erase(0, pos);
		else
			line.erase(0, pos + 1);
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
	if (r.getProtocolV()->compare(0, 8, "HTTP/1.1"))
		throw Request::BadRequest();
}

void	parse_line(std::string & line, Request& r)
{
	if ((line.compare(0, 3, "GET") ||
		line.compare(0, 4, "POST") ||
		line.compare(0, 6, "DELETE")) && !r.getMethod())
	{
		parse_method(line, r);
		check_method(r);
	}
}

void	parse_request(std::istringstream& request, Request &r)
{
	std::string	line;

	while (!request.eof())
	{
		std::getline(request, line);
		parse_line(line, r);
	}
}
