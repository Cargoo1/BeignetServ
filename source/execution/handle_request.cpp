/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_request.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ratel <ratel@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/11 19:40:38 by alejandroca       #+#    #+#             */
/*   Updated: 2026/06/16 16:07:16 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include "serverConfig.hpp"
#include "ExecutionContext.hpp"
#include "HttpResponse.hpp"
#include "utils.hpp"
#include <cstdlib>
#include <iostream>
#include <map>
#include <netinet/in.h>
#include <sstream>
#include <string>
#include <parse_request.hpp>
#include <sys/socket.h>
#include <vector>
#include <Client.hpp>
#include <serverConfig.hpp>
#include <send_http_response.hpp>

// namespace {
// 	int validateContex(const Request &req, const locationConfig &loc, const serverConfig &serv) {

// 	}
// }

serverConfig const&	find_server_block(Client& client, std::vector<serverConfig> const& serverConfig)
{
	std::string	ip_port = client.getIp() + ':' + client.getPort();
	std::vector<class serverConfig>::const_iterator it;
	for (it = serverConfig.begin(); it != serverConfig.end(); it++)
	{
		if (ip_port.compare(it->listen()) == 0)
			return *it;
	}
	for (it = serverConfig.begin(); it != serverConfig.end(); it++)
	{
		if (client.getPort().compare(it->listen()) == 0)
			return *it;
	}
	return serverConfig.front();
}

int	read_body(Request& r, std::istringstream& request_stream)
{
	std::string	line;
	while (r.getBytesRead() < r.getBodyLen() && std::getline(request_stream, line))
	{
		std::cout << "Body Line read: " + line + '\n';
		r.appendBody(line);
		if (!r.addBytesRead(line.length()))
		{
			std::cerr << "Bytes read limit reached, couldnt add more\n";
			return -1;
		}
	}
	if (r.getBytesRead() == r.getBodyLen())
		return 0;
	return r.getBytesRead();
}

int	handle_request(Client& client, std::vector<serverConfig> const& serverConf)
{
	
	Request&	r = client.getRequest();
	serverConfig const& server_block = find_server_block(client, serverConf);
	std::cout << "Server block: " + server_block.listen() + '\n';
	std::istringstream	request_stream(client.getRequest().getMessage());
	if (!r.getHeader().is_header_parsed())
	{
		try
		{
			parse_header(request_stream, r.getHeader());
		}
		catch(Request::ErrorRequest& e)
		{
			//send_response(r, e.getErrorCode(), server_block, client.getFd(), loc_block);
			return -1;
		}
	}
	std::map<std::string, std::string> const&	fields = r.getHeader().getFields();
	locationConfig const&	loc_block = find_location_block(r.getHeader().getTargetResource(), server_block);
	
	if (fields.find("Content-Length") == fields.end())
	{
		send_response(r, 200, server_block, client.getFd(), loc_block);
		return 0;
	}
	r.setBodyLen(ft_atoull(fields.at("Content-Length").c_str()));
	if (read_body(r, request_stream) == 0)
	{
		send_response(r, 200, server_block, client.getFd(), loc_block);
		std::cout << "sending response, done reading the body\n";
		//send_response(r, 200, server_block, client.getFd());
	}
	return 0;
}




	// if (fields.find("Content-Length") == fields.end())
	// {
	// 	send_response(r, 200, server_block, client.getFd(), loc_block);
	// 	return 0;
	// }
	// r.setBodyLen(ft_atoull(fields.at("Content-Length").c_str()));
	// if (read_body(r, request_stream) == 0)
	// {
	// 	send_response(r, 200, server_block, client.getFd(), loc_block);
	// 	std::cout << "sending response, done reading the body\n";
	// 	//send_response(r, 200, server_block, client.getFd());
	// }
	// return 0;