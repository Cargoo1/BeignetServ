/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_request.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ratel <ratel@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/11 19:40:38 by alejandroca       #+#    #+#             */
/*   Updated: 2026/07/25 14:45:39 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include <cstdlib>
#include <cstring>
#include <send_http_response.hpp>
#include "HttpResponse.hpp"
#include "Server.hpp"
#include "run_server.hpp"
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
#include <HttpMethodDispatcher.hpp>
#include <send_http_response.hpp>

// namespace {
// 	int validateContex(const Request &req, const locationConfig &loc, const serverConfig &serv) {

// 	}

int	handle_request(Client& client, Server& server)
{
	if (!client.getRequest().getReqInProg())
	{
		client.getRequest().setReqInProg(true);
		client.getRequest().setServerBlock(find_server_block(client, server.getServerConf()));
	}
	Request&	r = client.getRequest();
	print_log(TEXT_CYAN, NULL, "CLIENT INPUT:\n@@@@@@@@@@\n>>>" +
								client.getMsg() +
								"<<<\n@@@@@@@@@@\n\n", 0);
	if (!r.getHeader().is_header_parsed())
	{
		try
		{
			if (parse_fields(client.getNotConstMsg(), r) == INCOMPLETE)
				return 1;
			if (r.getHeader().getFields().find("Host") == r.getHeader().getFields().end())
				throw Request::ErrorRequest(bad_request, "Host field missing");
			r.getHeader().set_is_header_parsed(true);
		}
		catch(Request::ErrorRequest& e)
		{
			print_log(TEXT_RED, &e, e.what(), 1);
			r.getResponse().setStatusCode(e.getErrorCode());
			client.error_request = true;
			return -1;
		}
	}
	if (!r.getLocConfBlock() || !r.getServerBlock())
	{
		r.getResponse().setStatusCode(internal_server_error);
		client.error_request = true;
		return -1;
	}
	try
	{
		if (parse_body(r, client.getNotConstMsg()) == INCOMPLETE)
			return INCOMPLETE;
	}
	catch (Request::ErrorRequest& e)
	{
		print_log(TEXT_RED, &e, e.what(), 1);
		r.getResponse().setStatusCode(e.getErrorCode());
		client.error_request = true;
		return -1;
	}
	int infno = router(r, r.getResponse(), server);
	if (infno == RUN_CGI)
		return server.addCgiChild(client);
	else if (infno != 0)
	{
		client.error_request = true;
		return infno;
	}
	r.is_request_done = true;
	return DONE;
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
