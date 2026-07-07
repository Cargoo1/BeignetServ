/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_request.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ratel <ratel@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/11 19:40:38 by alejandroca       #+#    #+#             */
/*   Updated: 2026/07/07 18:02:56 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include <send_http_response.hpp>
#include "HttpResponse.hpp"
#include "utils.hpp"
#include "utils_logs.hpp"
#include <iostream>
#include <map>
#include <netinet/in.h>
#include <sstream>
#include <string>
#include <parse_request.hpp>
#include <parse_body.hpp>
#include <sys/socket.h>
#include <Client.hpp>
#include <serverConfig.hpp>
#include <send_http_response.hpp>

// namespace {
// 	int validateContex(const Request &req, const locationConfig &loc, const serverConfig &serv) {

// 	}

int	handle_request(Client& client)
{
	HttpResponse response;
	Request&	r = client.getRequest();
	std::stringstream	request_stream(client.getMsg());
	print_log(TEXT_CYAN, NULL, "CLIENT INPUT:\n------------------------\n" +
								client.getMsg() +
								"------------------------\n\n", 0);
	if (!r.getHeader().is_header_parsed())
	{
		try
		{
			parse_header(request_stream, r);
		}
		catch(Request::ErrorRequest& e)
		{
			print_log(TEXT_RED, &e, e.what(), 1);
			send_response(r, response, client.getFd(), e.getErrorCode());
			return -1;
		}
	}
	if (!r.getLocConfBlock() || !r.getServerBlock())
	{
		send_response(r, response, client.getFd(), internal_server_error);
		return -1;
	}
	int	return_value = parse_body(r, client.getNotConstMsg());
	if (return_value > 1)
	{
		send_response(r, response, client.getFd(), return_value);
		return 0;
	}
	else if (return_value == 1)
		return 0;
	send_response(r, response, client.getFd(), 0);
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
