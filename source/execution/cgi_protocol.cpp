/* *************n************************************************************ */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi_protocol.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/30 19:17:45 by acamargo          #+#    #+#             */
/*   Updated: 2026/08/03 20:04:23 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include <parserUtils.hpp>
#include <run_server.hpp>
#include <utils.hpp>
#include <utils_logs.hpp>
#include <map>
#include <parse_request.hpp>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/wait.h>

int	handle_script_output(CgiChild& child)
{
	Request&	r = child.getClientOwner().getRequest();
	Request		cgi_temp_request;
	cgi_temp_request.getHeader().is_method_parsed = true;
	cgi_temp_request = r;
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
	return 0;
}

int	get_script_output(Server& server, int pipe_fd)
{
	std::string	msg;
	CgiChild&	child = server.getCgiChilds().at(pipe_fd);
	if (read_msg(msg, child.getReadFromScriptPipe()[0]) > 0)
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
		server.removeChild(child.getReadFromScriptPipe()[0]);
	return 0;
}
