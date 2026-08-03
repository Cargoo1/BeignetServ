/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/03 17:39:25 by acamargo          #+#    #+#             */
/*   Updated: 2026/08/03 16:22:41 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <cstddef>
#include <string>

#include <sstream>

#include <Client.hpp>

#include <vector>

#include <string>

#include <cstring>

#include "Server.hpp"

#include "serverConfig.hpp"

#include <CgiChild.hpp>

enum MIME { JPG, PNG, HTML, CSS,TXT, CGI_PY, APP};

#define BUFF_SIZE 256

#define NULL_STR ""

#define CGI_DIR "/cgi-bin"

#define EXTENSIONS_SIZE 4

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
	internal_server_error = 500,
	http_version_not_supported = 505,
	bad_geteway = 502
};

typedef enum	e_scripts_ext
{
	INVALID = -1,
	PYTHON,
	CGI_EXT,
	WS
}	t_scripts_ext;

std::string findExt(const std::string &path);

bool	decode_percent_encoding(std::string& str, size_t& pos);

bool	is_unreserved(char c);

MIME find_type(const std::string &filePath);

std::string	find_content_type(const std::string &filePath);

std::string const generate_reason_phrase(int code);

std::vector<std::string> splitPath(const std::string path);

std::string getQuery(const std::string path);

std::string getQuery_path(const std::string path);

int		recv_msg(std::string& str, int cfd);

bool	is_in_cgi_dir(std::string const& uri);

unsigned long long	ft_atoull(char const* str);

locationConfig const&	find_location_block(std::string const& uri, serverConfig const& server);

serverConfig const&	find_server_block(Client& client, std::vector<serverConfig> const& serverConfig);

void	ft_itoa(int n, std::string& str);

int	split(std::vector<std::string>& vector, std::string const& str, char c);

int	hex_to_int(std::string str);

bool	remove_cr(std::string& str);

void	consume_until_crlf(std::string& str);

bool	is_a_cgi_request(std::string const& uri, size_t& path_extra_pos);

int		get_script_output(Server& server, int pipe_fd);

int	execute_script(Request const& r, HttpResponse& response);

int		read_msg(std::string& str, int fd);

void	free_double_char_ptr(char **ptr);

char	*ft_create_c_str(std::string const& str);

void	find_extension(std::string const& file_path, std::string& extension);

void ft_handler(int sig);

void	find_ip_and_port(std::string& ip, std::string& port, std::string const& server_listen);

bool parseIP(const std::string &ipstr);