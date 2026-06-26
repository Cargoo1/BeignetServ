/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_request.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ratel <ratel@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/11 19:40:38 by alejandroca       #+#    #+#             */
/*   Updated: 2026/06/23 17:32:10 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include "serverConfig.hpp"
#include "ExecutionContext.hpp"
#include "HttpResponse.hpp"
#include "HttpMethodDispatcher.hpp"
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

int	handle_request(Client& client)
{
	HttpResponse rsp;
	Request&	r = client.getRequest();
	std::istringstream	request_stream(client.getRequest().getMessage());
	locationConfig	loc_block = find_location_block(r.getHeader().getTargetResource(), server_block);
	// std::cout << "LOG: DEBUG:" << std::endl;
	// std::cout << loc_block.getMethod().at(0) << std::endl;
	ExecutionContext context(r, loc_block, server_block);
	if (!r.getHeader().is_header_parsed())
	{
		try
		{
			parse_header(request_stream, r.getHeader());
		}
		catch(Request::ErrorRequest& e)
		{
			send_response(context, rsp, client.getFd());
			return -1;
		}
	}
	if (!r.getLocConfBlock() || !r.getServerBlock())
		return -1;
	std::map<std::string, std::string> const&	fields = r.getHeader().getFields();
	if (fields.find("Content-Length") == fields.end())
	{
		send_response(r, 200, *r.getServerBlock(), client.getFd(), *r.getLocConfBlock());
		return 0;
	}
	if (read_body(r, request_stream) == 0)
	{
		send_response(r, 200, *r.getServerBlock(), client.getFd(), *r.getLocConfBlock());
		std::cout << "sending response, done reading the body\n";
	}
	rsp = router(context);
	send_response(context, rsp, client.getFd());
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
