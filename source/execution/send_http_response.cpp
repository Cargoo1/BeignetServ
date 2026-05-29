/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   send_http_response.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/21 21:54:03 by acamargo          #+#    #+#             */
/*   Updated: 2026/05/29 13:55:57 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <HttpResponse.hpp>
#include <cerrno>
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
#include <Client.hpp>

std::string const getReasonPhrase(int code)
{
	std::string	ret;
	switch (code)
	{
		
		case 201:
			ret = " Created";
			break;
		case 301: case 302:
			ret = " Redirect";
			break;
		case bad_request:
			ret = " Bad Request";
			break;
		case forbiden:
			ret = " Forbiden";
			break;
		case not_found:
			ret = " Not Found";
			break;
		case method_not_allowed:
			ret = " Method Not Allowed";
			break;
		case payload_too_large:
			ret = " Payload Too Large";
			break;
		case 500:
			ret = " Payload Too Large";
			break;
		default:
			ret = " OK";
			break;
	}
	return (ret);
}

std::string	generate_default_error_page(HttpResponse& response)
{
	std::stringstream	error_code;
	error_code << response.getStatusCode() << getReasonPhrase(response.getStatusCode());
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
std::string findExt(const std::string &path)
{
	std::string ret = path;
	std::string del = ".";
	std::string::size_type pos = ret.find(del);
	while (pos != std::string::npos) {
		ret.erase(0, pos + del.length());
		pos = ret.find(del);
	}
	return (ret);
} 

MIME find_type(const std::string &filePath)
{
	std::string ext = findExt(filePath);
	if (ext == "jpeg")
		return (JPG);
	else if (ext == "png")
		return (PNG);
	else if (ext == "html")
		return (HTML);
	else if (ext == "txt")
		return (TXT);
	else
		return (APP);
} 
std::string	find_content_type(const std::string &filePath)
{
	MIME ext = find_type(filePath);
	switch (ext)
	{
		case JPG:
			return ("image/jpeg");
		case PNG:
			return ("image/png");
		case HTML:
			return ("text/html");
		case TXT:
			return ("text/plain");
		default:
			return ("application/octet-stream");
	}
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

void	send_error_response(HttpResponse& response, serverConfig const& serverConf, int error_code)
{
	std::map<int, std::string>::const_iterator	it = serverConf._errorPages.find(response.getStatusCode());

	if (it == serverConf._errorPages.end())
	{
		create_default_error_response(response, error_code);
		return;
	}
	if (access(it->second.c_str(), F_OK) == 0)
	{
		if (access(it->second.c_str(), R_OK) == 0)
		{
			create_error_response(response, error_code, it->second);
		}
		else
			create_default_error_response(response, forbiden);
	}
	else
		create_default_error_response(response, not_found);
}

int	sendall(int fd, char const* buf, int &len)
{
	int	bytes_sent = 0;
	int	bytes_left = len;
	int	total_bytes_sent = 0;

	while (total_bytes_sent < len)
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

int	send_response(Client const& client, int status_code, serverConfig const& serverConf)
{
	HttpResponse	response;
	std::string		msg;
	response.addField("Server", "Beignetserv/0.1");
	if (status_code >= 400)
		send_error_response(response, serverConf, status_code);
	else
	{
		create_default_error_response(response, 200);
	}
	msg = response.toHttpString();
	int	len = msg.length();
	sendall(client.getFd(), msg.c_str(), len);
	if (static_cast<size_t>(len) != msg.length())
	{
		std::cerr << "Failed sending all bytes in the response\n";
		return -1;
	}
	return 0;
}
