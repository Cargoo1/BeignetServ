/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_cgi.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/16 20:36:21 by acamargo          #+#    #+#             */
/*   Updated: 2026/07/16 21:00:47 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstddef>
#include <utils.hpp>

#define EXTENSIONS_SIZE 3

bool	is_in_cgi_dir(std::string const& uri)
{
	/*
	   /cgi_bin 
	 /cgi_bin/
	 /cgi_bin/yes.py
	 */
	size_t dir_pos = uri.find(CGI_DIR);
	if (dir_pos == std::string::npos)
		return false;
	dir_pos += std::strlen(CGI_DIR) - 1;
	if (dir_pos > uri.length())
		return false;
	else if (dir_pos == uri.length())
		return true;
	else if (uri.at(dir_pos + 1) != '/')
		return false;
	return true;
}

bool	is_a_cgi_request(std::string const& uri, size_t& path_extra_pos)
{
	std::string	cgi_extensions[EXTENSIONS_SIZE] = {".py", ".cgi", ".ws"};
	size_t	ext_pos = 0;

	for (size_t i = 0; i < EXTENSIONS_SIZE; i++)
	{
		ext_pos = uri.find(cgi_extensions[i]);
		ext_pos += cgi_extensions[i].length() - 1;
		if (uri.length() < ext_pos)
			continue;
		else if (ext_pos == uri.length())
		{
			path_extra_pos = uri.length();
			return true;
		}
		else if (uri.at(ext_pos + 1) == '/')
		{
			path_extra_pos = ext_pos + 1;
			return true;
		}
	}
	if (is_in_cgi_dir(uri))
		return true;
	return false;
}
