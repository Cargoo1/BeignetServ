/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   send_http_response.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/21 21:54:03 by acamargo          #+#    #+#             */
/*   Updated: 2026/06/29 13:08:57 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ExecutionContext.hpp"
#include "GetMethod.hpp"
#include "HttpMethodDispatcher.hpp"
#include "PostMethod.hpp"
#include "DeleteMethod.hpp"
#include "Request.hpp"
#include "locationConfig.hpp"
#include <HttpResponse.hpp>

#include <cerrno>
#include <cstddef>
#include <cstring>
#include <fstream>
#include <map>
#include <ostream>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <serverConfig.hpp>
#include <utils.hpp>
#include <Client.hpp>
#include <send_http_response.hpp>

std::string	generate_default_error_page(HttpResponse& response)
{
	std::stringstream	error_code;
	error_code << response.getStatusCode() << generate_reason_phrase(response.getStatusCode());
	std::string error_page = "<html>\n<head><title>" + error_code.str() + "</title></head>\n<body>\n<center><h1>" + error_code.str() + "</h1></center>\n</body>\n</html>\n";
	return error_page;
}

void	create_default_error_response(HttpResponse& response, int error_code)
{
	response.setStatusCode(error_code);
	response.setBody(generate_default_error_page(response));
	response.addField("Content-Length", toStr(response.getBody().length()));
	response.addField("Content-Type", "text/html");
}

void	create_error_response(HttpResponse& response, int error_code, std::string const& file_name)
{
	std::ifstream	ifs;
	std::stringstream		body;

	ifs.open(file_name.c_str());
	if (!ifs.is_open())
	{
		create_default_error_response(response, internal_server_error);
		return;
	}
	response.setStatusCode(error_code);
	body << ifs.rdbuf();
	response.setBody(body.str());
	response.addField("Content-Length", toStr(response.getBody().length()));
	response.addField("Content-Type", find_content_type(file_name));
}

void	send_error_response(HttpResponse& response, serverConfig const& serverConf)
{
	std::map<int, std::string>::const_iterator	it = serverConf.errorPages().find(response.getStatusCode());

	if (it == serverConf.errorPages().end())
	{
		create_default_error_response(response, response.getStatusCode());
		return;
	}
	if (access(it->second.c_str(), F_OK) == 0)
	{
		if (access(it->second.c_str(), R_OK) == 0)
		{
			create_error_response(response, response.getStatusCode(), it->second);
		}
		else
			create_default_error_response(response, forbiden);
	}
	else
		create_default_error_response(response, not_found);
}

int	sendall(int fd, char const* buf, size_t &len)
{
	int	bytes_sent = 0;
	int	bytes_left = len;
	int	total_bytes_sent = 0;

	while (total_bytes_sent < static_cast<int>(len))
	{
		bytes_sent = send(fd, buf + total_bytes_sent, bytes_left, 0);
		if (bytes_sent < 0)
		{
			std::cerr << "Fail sending response to the client, " << strerror(errno) << '\n';
			return -1;
		}
		bytes_left -= len;
		total_bytes_sent += bytes_sent;
	}
	len = total_bytes_sent;
	return 0;
}

int	send_response(Request& r, HttpResponse &response, int cfd, int status_code)
{
	if (status_code)
		response.setStatusCode(status_code);
	else
		router(r, response);
	std::string msg;
	response.addField("Server", "Beignetserv/0.1");
	if (response.getStatusCode() >= 400)
		send_error_response(response, *r.getServerBlock());
	msg = response.toHttpString();
	size_t	len = msg.length();
	sendall(cfd, msg.c_str(), len);
	r = Request();
	if (len != msg.length())
	{
		std::cerr << "Failed sending all bytes in the response\n";
		return -1;
	}
	return 0;
}

// int	send_response(Request& r, int status_code, serverConfig const& server_block, int cfd, 
// 					locationConfig const& loc_block)
// {
// 	(void)loc_block;
// 	HttpResponse	response;
// 	std::string		msg;
// 	response.addField("Server", "Beignetserv/0.1");
// 	if (status_code >= 400)
// 		send_error_response(response, server_block, status_code);
// 	msg = response.toHttpString();
// 	int	len = msg.length();
// 	sendall(cfd, msg.c_str(), len);
// 	r = Request();
// 	if (static_cast<size_t>(len) != msg.length())
// 	{
// 		std::cerr << "Failed sending all bytes in the response\n";
// 		return -1;
// 	}
// 	return 0;
// }
