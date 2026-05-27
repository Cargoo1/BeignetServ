/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_request.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandrocamargo <acamargo@student.42.fr>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/11 19:40:38 by alejandrocama     #+#    #+#             */
/*   Updated: 2026/05/27 16:42:34 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include <cerrno>
#include <cstring>
#include <exception>
#include <iostream>
#include <netinet/in.h>
#include <sstream>
#include <stdexcept>
#include <string>
#include <parse_request.hpp>
#include <sys/socket.h>
#include <vector>
#include <Client.hpp>
#include <serverConfig.hpp>
#include <send_http_response.hpp>

serverConfig const&	find_server_block(Client& client, std::vector<serverConfig> const& serverConfig)
{
	std::string	ip_port = client.getIp() + ':' + client.getPort();
	std::vector<class serverConfig>::const_iterator it;
	for (it = serverConfig.begin(); it != serverConfig.end(); it++)
	{
		if (ip_port.compare(it->_listen) == 0)
			return *it;
	}
	for (it = serverConfig.begin(); it != serverConfig.end(); it++)
	{
		if (client.getPort().compare(it->_listen) == 0)
			return *it;
	}
	return serverConfig.front();
}

int	handle_request(Client& client, std::vector<serverConfig> const& serverConf)
{
	if (client.getMessage().find("\r\n\r\n") == std::string::npos)
		return 0;
	serverConfig const& server_block = find_server_block(client, serverConf);
	std::cout << "Server block: " + server_block._listen + '\n';
	std::istringstream	request_stream(client.getMessage());
	Request	&r = client.getRequest();
	if (r.getHeader().getMethod().empty())
	{
		try
		{
			parse_header(request_stream, r.getHeader());
		}
		catch(Request::ErrorRequest& e)
		{
			send_response(client, e.getErrorCode(), server_block);
			return -1;
		}
	}
	send_response(client, 200, server_block);
	if (r.getHeader().getFields().find("Content_Length") != r.getHeader().getFields().end())
	{
		//
	}
	return 0;	
}
