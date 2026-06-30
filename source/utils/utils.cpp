/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/03 17:41:28 by acamargo          #+#    #+#             */
/*   Updated: 2026/06/30 22:22:07 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "serverConfig.hpp"
#include <cctype>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <map>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <Client.hpp>
#include <sys/types.h>
#include <utils.hpp>
#include <vector>

#define CGI_DIR "/cgi_bin\0"

std::string const generate_reason_phrase(int code)
{
	std::string	ret;
	switch (code)
	{
		
		case 201:
			ret = "Created";
			break;
		case 301: case 302:
			ret = "Redirect";
			break;
		case bad_request:
			ret = "Bad Request";
			break;
		case forbiden:
			ret = "Forbiden";
			break;
		case not_found:
			ret = "Not Found";
			break;
		case method_not_allowed:
			ret = "Method Not Allowed";
			break;
		case payload_too_large:
			ret = "Payload Too Large";
			break;
		case 500:
			ret = "Payload Too Large";
			break;
		default:
			ret = "OK";
			break;
	}
	return (ret);
}

std::vector<std::string> splitPath(const std::string path) {
	std::string part;
	std::istringstream			iss(path);
	std::vector<std::string>	ret;

	while (std::getline(iss, part, '/'))
		if (!part.empty()) {
			ret.push_back(part);
	}
	return (ret);
}

std::string findExt(const std::string &path) {
	std::string ret = path;
	std::string del = ".";
	std::string::size_type pos = ret.find_last_of(del);
	while (pos != std::string::npos) {
		ret.erase(0, pos + del.length());
		pos = ret.find(del);
	}
	return (ret);
}

char	hex_to_char(std::string str)
{
	int	c = 0;
	size_t	found;
	int		exponent = 0;
	std::string	hexadecimal= "0123456789abcdef";
	for (int i = str.length() - 1; i >= 0; --i)
	{
		found = hexadecimal.find(std::tolower(str.at(i)));
		if (found == std::string::npos)
			return 0;
		c = c + found * static_cast<int>(std::pow(16, exponent++));
	}
	if (c > 127 || c < 0)
		return 0;
	return c;
}

int	hex_to_int(std::string str)
{
	int	n = 0;
	size_t	found;
	int		exponent = 0;
	std::string	hexadecimal= "0123456789abcdef";
	for (int i = str.length() - 1; i >= 0; --i)
	{
		found = hexadecimal.find(std::tolower(str.at(i)));
		if (found == std::string::npos)
			return 0;
		n = n + found * static_cast<int>(std::pow(16, exponent++));
	}
	return n;
}

bool	is_hexadecimal(std::string const& str)
{
	for (size_t i = 0; i < str.length(); ++i)
	{
		if (std::isalpha(str.at(i)) && str.at(i) <= 'F')
			return true;
	}
	return false;
}

bool	is_unreserved(char c)
{
	if (std::isalnum(c)
		|| c == '-'
		|| c == '_'
		|| c ==	'~'
		|| c == '.')
		return true;
	return false;
}

bool	decode_percent_encoding(std::string& str, size_t& pos)
{
	char	c;
	int		encoding_len = 0;
	std::string	encoded_str;
	if (str.at(pos) != '%')
		return false;
	size_t	i = pos + 1;
	while (i < str.length() && i < pos + 3)
	{
		if (!isalnum(str.at(i)))
			break;
		++i;
		++encoding_len;
	}
	encoded_str = str.substr(pos + 1, encoding_len);
	c = hex_to_char(encoded_str);
	if (c == 0)
		return false;
	str.replace(pos, 3, 1, c);
	++pos;
	return true;
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
	else if (ext == "css")
		return (CSS);
	else if (ext == "txt")
		return (TXT);
	else if (ext == "py")
		return (CGI_PY);
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
		case CSS:
			return ("text/css");
		case TXT:
			return ("text/plain");
		default:
			return ("application/octet-stream");
	}
}

std::string getQuery(const std::string path) {
	std::size_t found = path.find_first_of("?");
	if (found == std::string::npos) {
		return (NULL_STR);
	}
	std::string query = path.substr(found+1);
	query = query.substr(0, query.find_first_of(" "));
	return (query);
}

std::string getQuery_path(const std::string path) {
	std::string newPath = path.substr(path.find_first_of("/"));
	newPath = newPath.substr(0, newPath.find_first_of("?"));
	return (newPath);
}

bool	listen_msg(std::string& str, int cfd)
{
	char	buff[BUFF_SIZE];

	std::memset(buff, 0, BUFF_SIZE);
	int		size_read = recv(cfd, &buff, BUFF_SIZE - 1, 0);
	if (size_read <= 0)
		return false;
	buff[size_read] = '\0';
	str.append(buff);
	return true;
}

bool	is_in_cgi_dir(std::string const& uri)
{
	/*
	   /cgi_bin 
	 /cgi_bin/
	 /cgi_bin/yes.py
	 */
	size_t dir_pos = uri.find(CGI_DIR, 0);
	if (dir_pos == std::string::npos)
		return false;
	dir_pos += std::strlen(CGI_DIR);
	if (dir_pos > uri.length())
		return false;
	if (dir_pos < uri.length() && uri.at(dir_pos) != '/')
		return false;
	/*
	size_t dir_pos = uri.find_last_of('/');
	if (dir_pos == uri.length() - 1)
		dir_pos = uri.find_last_of('/', dir_pos - 1);
	if (uri.compare(dir_pos + 1, std::strlen(CGI_DIR) + 1, CGI_DIR) == 0)
		return true;
	return false;
	*/
	return true;
}

int	split(std::vector<std::string>& vector, std::string const& str, char c)
{
	size_t	i = 0, k;
	std::string	str_tmp;
	k = i;
	while (i < str.size())
	{
		if (str.at(i) == c)
		{
			if (i == 0)
			{
				++i;
				continue;
			}
			vector.push_back(str.substr(k, (i  - k)));
			while (i < str.length() && str.at(i) == c)
				++i;
			k = i;
		}
		++i;
	}
	vector.push_back(str.substr(k, (i  - k)));
	return 0;
}
int	split_between_delimiter(std::vector<std::string>& vector, std::string const& str, char c)
{
	size_t	i = 0, k, j;
	std::string	str_tmp;
	while (i < str.size())
	{
		k = std::string::npos;
		j = std::string::npos;
		if (str.at(i) == c)
		{
			k = i++;
			while (i < str.size() && str.at(i) != c)
				++i;
			j = i;
			if (j >= str.size())
				continue;
			if (k + 1 >= str.size())
				continue;
			str_tmp = str.substr(k + 1, j);
			vector.push_back(str_tmp);
		}
		++i;
	}
	return 0;
}

serverConfig const&	find_server_block(Client& client, std::vector<serverConfig> const& serverConfig)
{
	std::string	ip_port = client.getIp() + ':' + client.getPort();
	std::vector<class serverConfig>::const_iterator it;
	for (it = serverConfig.begin(); it != serverConfig.end(); it++)
	{
		if (ip_port.compare(it->listen()) == 0)
			return *it;
	}
	for (it = serverConfig.begin(); it != serverConfig.end(); it++)
	{
		if (client.getPort().compare(it->listen()) == 0)
			return *it;
	}
	return serverConfig.front();
}

locationConfig const&	find_location_block(std::string const& uri, serverConfig const& server)
{
	std::vector<locationConfig> const&	loc_confs = server.locations();
	size_t	longest_dirs_matched = 0;
	size_t	directories_matched = 0;
	int		longest_match = -1;
	size_t	slash_pos = std::string::npos;
	size_t	j = 0;
	size_t	k = 0;
	for (size_t i = 0; i < loc_confs.size(); ++i)
	{
		std::string	const& loc_path = loc_confs.at(i).getPath();

		directories_matched = 0;
		j = 0;
		k = 0;
		while (j < uri.length() && k < loc_path.length())
		{
			slash_pos = loc_path.find('/', k);
			if (slash_pos == std::string::npos)
				slash_pos = loc_path.size();
			else
				++slash_pos;
			while (j < slash_pos && j < uri.length())
			{
				if (uri.at(j) != loc_path.at(k))
					break;
				++j;
				++k;
			}
			if (j < slash_pos)
				break;
			++directories_matched;
		}
		if (directories_matched > longest_dirs_matched)
		{
			longest_dirs_matched = directories_matched;
			longest_match = i;
		}
	}
	if (longest_match < 0)
		return loc_confs.at(0);
	return loc_confs.at(longest_match);
}
