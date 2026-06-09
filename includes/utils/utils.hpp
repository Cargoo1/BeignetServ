/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/03 17:39:25 by acamargo          #+#    #+#             */
/*   Updated: 2026/06/09 14:32:17 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <sstream>

enum MIME { JPG, PNG, HTML, TXT, APP};

#define BUFF_SIZE 256

#define NULL_STR ""

template <typename T> 
std::string toStr(T nbr) {
	std::ostringstream os;
	os << nbr;
	std::string ret = os.str();
	return (ret);
}

enum	client_error
{
	bad_request = 400,
	forbiden = 403,
	not_found,
	method_not_allowed,
	payload_too_large = 415,
	content_too_large = 413,
	internal_server_error = 500
};

std::string findExt(const std::string &path);

bool	decode_percent_encoding(std::string& str, size_t& pos);

bool	is_unreserved(char c);

MIME find_type(const std::string &filePath);

std::string	find_content_type(const std::string &filePath);

std::string const generate_reason_phrase(int code);

std::string getQuery(const std::string path);

std::string getQuery_path(const std::string path);

bool	listen_msg(std::string& str, int cfd);
