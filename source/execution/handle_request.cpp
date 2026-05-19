/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_request.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandrocamargo <acamargo@student.42.fr>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/11 19:40:38 by alejandrocama     #+#    #+#             */
/*   Updated: 2026/05/19 21:11:37 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include <exception>
#include <iostream>
#include <sstream>
#include <string>
#include <parse_request.hpp>

int	handle_request(Client& client)
{
	if (client.getMessage().find("\r\n\r\n") == std::string::npos)
		return 0;
	std::istringstream	request_stream(client.getMessage());
	Request	&r = client.getRequest();
	if (r.getHeader().getMethod().empty())
	{
		try
		{
			parse_header(request_stream, r.getHeader());
		}
		catch(std::exception& e)
		{
			std::cerr << e.what();
			return -1;
		}
	}
	if (r.getHeader().getFields().find("Content_Length") == r.getHeader().getFields().end())
	{
		//send_response
	}
	std::cout << r.getHeader().getMethod() << std::endl;
	std::cout << r.getHeader().getProtocolV()<< std::endl;
	std::cout << r.getHeader().getTargetResource() << std::endl;
	std::cout << r.getHeader().getFields().at("Host") << '\n';
	return 200;	
}
