/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_request.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/07 19:49:10 by acamargo          #+#    #+#             */
/*   Updated: 2026/05/09 13:21:42 by acamargo         ###   ########.fr       */
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
	if (pos == std::string::npos)
		throw Request::BadRequest();
	if (line.compare(0, pos - 1, "GET"))
		throw Request::BadRequest();
	line.erase(0, pos + 1);
	
}

void	parse_line(std::string & line, Request& r)
{
	if ((line.compare(0, 3, "GET") ||
		line.compare(0, 4, "POST") ||
		line.compare(0, 6, "DELETE")) && !r.getMethod())
	{
		
	}
}

Request	parse_request(std::istringstream& request)
{
	std::string	method, target_ressource, protocol_v;
	std::map<std::string, std::string> host;
	std::string	line;
	while (!request.eof())
	{
		std::getline(request, line);

	}	
}
