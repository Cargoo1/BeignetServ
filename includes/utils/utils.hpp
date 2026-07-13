/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/03 17:39:25 by acamargo          #+#    #+#             */
/*   Updated: 2026/07/13 17:06:35 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>

#include <sstream>

#include <Client.hpp>

#include <vector>

#include "serverConfig.hpp"

enum MIME { JPG, PNG, HTML, CSS,TXT, CGI_PY, APP};

#define BUFF_SIZE 256

#define NULL_STR ""

template <typename T> 
std::string toStr(T nbr) {
	std::ostringstream os;
	os << nbr;
	std::string ret = os.str();
	return (ret);
}

template <typename T> 
void toDigit(std::string nbr, T &out) {
	std::stringstream ss;
	ss << nbr;
	ss >> out;
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

std::vector<std::string> splitPath(const std::string path);

std::string getQuery(const std::string path);

std::string getQuery_path(const std::string path);

bool	recv_msg(std::string& str, int cfd);

bool	is_in_cgi_dir(std::string const& uri);

unsigned long long	ft_atoull(char const* str);

locationConfig const&	find_location_block(std::string const& uri, serverConfig const& server);

serverConfig const&	find_server_block(Client& client, std::vector<serverConfig> const& serverConfig);

void	ft_itoa(int n, std::string& str);

int	split(std::vector<std::string>& vector, std::string const& str, char c);

int	hex_to_int(std::string str);

bool	remove_cr(std::string& str);

void	consume_until_crlf(std::string& str);
