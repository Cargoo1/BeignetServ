/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_request.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ratel <ratel@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/11 19:40:38 by alejandroca       #+#    #+#             */
/*   Updated: 2026/08/02 20:06:56 by acamargo         ###   ########.fr       */
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

int	handle_request(Client& client, Server& server)
{
	if (!client.getRequest().is_request_in_progress)
	{
		client.getRequest().is_request_in_progress = true;
		client.getRequest().setServerBlock(find_server_block(client, server.getServerConf()));
	}
	Request&	r = client.getRequest();
	print_log(TEXT_BLUE, NULL, "Request:\n@@@@@@@@@@\n>>>" +
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
			server.set_2_epoll(EPOLLIN | EPOLLOUT, client.getFd(), EPOLL_CTL_MOD);
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
		server.set_2_epoll(EPOLLIN | EPOLLOUT, client.getFd(), EPOLL_CTL_MOD);
		return -1;
	}
	int infno = router(r, r.getResponse(), server);
	if (infno == RUN_CGI)
	{
		infno = server.addCgiChild(client);
		if (infno == INCOMPLETE)
			return infno;
	}
	else if (infno == INCOMPLETE)
		return infno;
	if (infno < 0)
		client.error_request = true;
	server.set_2_epoll(EPOLLIN | EPOLLOUT, client.getFd(), EPOLL_CTL_MOD);
	return DONE;
}
