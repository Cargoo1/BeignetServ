/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/03 17:41:28 by acamargo          #+#    #+#             */
/*   Updated: 2026/06/03 17:45:21 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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