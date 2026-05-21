/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_request.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandrocamargo <acamargo@student.42.fr>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/11 19:40:38 by alejandrocama     #+#    #+#             */
/*   Updated: 2026/05/21 23:20:38 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
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
#include <serverConfig.hpp>

serverConfig const&	find_server_block(int fd, std::vector<serverConfig> const& serverConfig)
{
	struct	sockaddr_in	sock_info;
	socklen_t			addr_len  = sizeof(sock_info);
	if (getsockname(fd, (struct sockaddr *)&sock_info, &addr_len) < 0)
		throw std::runtime_error(std::string("getsockname: ") + strerror(errno));
	std::stringstream	ss;
	ss << ntohs(sock_info.sin_port);
	std::string	sock_port = ss.str();
	return serverConfig.back();
}

int	handle_request(Client& client, std::vector<serverConfig> const& serverConf)
{
	if (client.getMessage().find("\r\n\r\n") == std::string::npos)
		return 0;
	find_server_block(client.getFd(), serverConf);
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
