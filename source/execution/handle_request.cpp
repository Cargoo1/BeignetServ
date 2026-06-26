/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_request.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ratel <ratel@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/11 19:40:38 by alejandroca       #+#    #+#             */
/*   Updated: 2026/06/26 19:57:10 by acamargo         ###   ########.fr       */
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
	HttpResponse response;
	Request&	r = client.getRequest();
	std::istringstream	request_stream(client.getRequest().getMessage());
	if (!r.getHeader().is_header_parsed())
	{
		try
		{
			parse_header(request_stream, r);
		}
		catch(Request::ErrorRequest& e)
		{
			response.setStatusCode(e.getErrorCode());
			send_response(r, response, client.getFd());
			return -1;
		}
	}
	if (!r.getLocConfBlock() || !r.getServerBlock())
	{
		response.setStatusCode(internal_server_error);
		send_response(r, response, client.getFd());
		return -1;
	}
	std::map<std::string, std::string> const&	fields = r.getHeader().getFields();
	if (fields.find("Content-Length") != fields.end())
	{
		if (read_body(r, request_stream) != 0)
			return 0;
	}
	router(r, response);
	send_response(r, response, client.getFd());
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
