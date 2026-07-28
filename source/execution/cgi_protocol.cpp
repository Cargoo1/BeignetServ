/* *************n************************************************************ */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi_protocol.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/30 19:17:45 by acamargo          #+#    #+#             */
/*   Updated: 2026/07/28 23:12:58 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiChild.hpp"
#include "Client.hpp"
#include "HttpResponse.hpp"
#include "Request.hpp"
#include "Server.hpp"
#include "locationConfig.hpp"
#include "parserUtils.hpp"
#include "run_server.hpp"
#include "send_http_response.hpp"
#include "utils.hpp"
#include "utils_logs.hpp"

#include <bits/stdc++.h>
#include <cerrno>
#include <cstddef>
#include <cstring>
#include <map>
#include <parse_request.hpp>

#include <sys/epoll.h>
#include <sys/socket.h>
#include <vector>
#include <sys/stat.h>
#include <sys/wait.h>

#define SCRIPT_TIMEOUT 20

int	handle_script_output(CgiChild& child)
{
	Request&	r = child.getClientOwner().getRequest();
	Request		cgi_temp_request;
	cgi_temp_request.getHeader().is_method_parsed = true;
	try
	{
		if (parse_fields(child.getOutput(), cgi_temp_request) == INCOMPLETE)
			throw Request::ErrorRequest(bad_geteway, "Not valid output");
	}
	catch (Request::ErrorRequest& e)
	{
		print_log(TEXT_RED, &e, e.what(), 1);
		r.getResponse().setStatusCode(bad_geteway);
		child.getClientOwner().error_request = true;
		return -1;
	}
	child.setLastComm();
	r.getResponse().setBody(child.getOutput());
	r.getResponse().setStatusCode(200);
	std::map<std::string, std::string>::iterator it = cgi_temp_request.getHeader().getFields().find("Status");
	if (it != cgi_temp_request.getHeader().getFields().end())
	{
		int	status = toInt(it->second);
		if (status > 505 || status < 200)
			status = bad_geteway;
		r.getResponse().setStatusCode(status);
	}
	r.is_request_done = true;
	return 0;
}

int	get_script_output(Server& server, int pipe_fd)
{
	std::string	msg;
	CgiChild&	child = server.getCgiChilds().at(pipe_fd);
	if (read_msg(msg, child.getOutputPipe()[0]) > 0)
	{
		child.setLastComm();
		child.appedOutput(msg);
		return 1;
	}
	child.is_done = true;
	int	infno = handle_script_output(child);
	server.set_2_epoll(EPOLLIN, pipe_fd, EPOLL_CTL_DEL);
	server.set_2_epoll(EPOLLIN | EPOLLOUT, child.getClientOwner().getFd(), EPOLL_CTL_MOD);
	if (infno == -1)
		server.removeChild(child.getOutputPipe()[0]);
	return 0;
}

void	check_idle_scripts(Server& server)
{
	time_t	curr_time = 0;

	std::time(&curr_time);
	if (curr_time - server.getLastCheckScripts() < 1)
		return;
	std::map<int, CgiChild>::iterator	it;
	for (it = server.getCgiChilds().begin(); it != server.getCgiChilds().end();)
	{
		if (!it->second.is_done)
		{
			if (curr_time - it->second.getLastComm() < SCRIPT_TIMEOUT)
			{
				++it;
				continue;
			}
			server.set_2_epoll(EPOLLIN | EPOLLOUT, it->second.getClientOwner().getFd(), EPOLL_CTL_MOD);
			it->second.getClientOwner().getRequest().getResponse().setStatusCode(bad_geteway);
			it->second.getClientOwner().error_request = true;
		}
		else
		{
			int return_value = waitpid(it->second.getPid(), NULL, WNOHANG);
			if (return_value > 0 || errno == ECHILD)
			{
				server.deleteCgiChild((it++)->first);
				continue ;
			}
			time_t curr;
			std::time(&curr);
			if (curr - it->second.getLastComm() < 10)
			{
				++it;
				continue;
			}

		}
		print_log(TEXT_YELLOW, NULL, "Removing iddle child from server: " + toStr(it->first), 0);
		server.removeChild((it++)->first);
	}
	server.setLastCheckScripts();
}
