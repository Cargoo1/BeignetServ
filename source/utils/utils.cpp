/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/03 17:41:28 by acamargo          #+#    #+#             */
/*   Updated: 2026/06/05 20:27:18 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cctype>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <map>
#include <iostream>
#include <utils.hpp>

std::string const generate_reason_phrase(int code)
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

std::string findExt(const std::string &path) {
	std::string ret = path;
	std::string del = ".";
	std::string::size_type pos = ret.find(del);
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
