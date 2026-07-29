/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   send_http_response.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ratel <ratel@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/21 21:54:03 by acamargo          #+#    #+#             */
/*   Updated: 2026/07/29 23:08:57 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ExecutionContext.hpp"
#include "GetMethod.hpp"
#include "HttpMethodDispatcher.hpp"
#include "PostMethod.hpp"
#include "DeleteMethod.hpp"
#include "Request.hpp"
#include "locationConfig.hpp"
#include "utils_logs.hpp"
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
	error_code << response.getStatusCode() << ' ' << generate_reason_phrase(response.getStatusCode());
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
	std::string final_path = "." + serverConf.getRoot() + it->second;
	if (access(final_path.c_str(), F_OK) == 0)
	{
		if (access(final_path.c_str(), R_OK) == 0)
		{
			create_error_response(response, response.getStatusCode(), final_path);
		}
		else
			create_default_error_response(response, forbiden);
	}
	else
		create_default_error_response(response, not_found);
}

int	send_response(Request& r, int cfd)
{
	HttpResponse&	response = r.getResponse();
	std::string msg;
	response.addField("Server", "Beignetserv/0.1");
	if (response.getStatusCode() >= 400)
		send_error_response(response, *r.getServerBlock());
	response.addField("Content-Length", toStr(response.getBody().length()));
	msg = response.toHttpString();
	size_t	len = msg.length();
	print_log(TEXT_MAGENTA, NULL, "SERVER OUTPUT:\n@@@@@@@@@@\n>>>" +
								msg +
								"<<<\n@@@@@@@@@@\n\n", 0);
	sendall(cfd, msg.c_str(), len);
	r = Request();
	if (len != msg.length())
	{
		std::cerr << "Failed sending all bytes in the response\n";
		return -1;
	}
	return 0;
}
