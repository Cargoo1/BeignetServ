/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tester.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 23:13:46 by acamargo          #+#    #+#             */
/*   Updated: 2026/08/03 17:12:01 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <ios>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>

typedef std::map<std::string, std::string> fields;

enum	methods
{
	GET,
	POST,
	DELETE
};

int	read_body(fields& fields, std::string& body)
{
	std::map<std::string, std::string>::iterator it = fields.find("CONTENT_LENGTH");
	if (it == fields.end())
		return 1;
	char	buff[250];
	memset(&buff, 0, sizeof(buff));
	long bytes_2_read = std::atoi(it->second.c_str());
	int		bytes_read = 0;
	while (bytes_2_read > 0)
	{
		bytes_read = read(0, buff, 250);
		if (bytes_read < 0)
			return -1;
		bytes_2_read -= bytes_read;
		body.append(buff, bytes_read);
	}
	return 0;
}

int	find_method(fields& header, std::string& method)
{
	fields::iterator it = header.find("REQUEST_METHOD");
	if (it == header.end())
		return -1;
	method = it->second;
	if (method == "GET")
		return GET;
	else if (method == "POST")
		return POST;
	else if (method == "DELETE")
		return DELETE;
	else
		return -1;
}

int	find_post_path(fields& header, std::string& path)
{
	fields::iterator	it;
	it = header.find("PATH_INFO");
	if (it == header.end())
		return -1;
	path = '.' + it->second;
	return 0;
}

int	find_filename(fields& header, std::string& filename)
{
	fields::iterator	it;
	it = header.find("QUERY_STRING");
	if (it != header.end() && !it->second.empty())
	{
		filename = it->second;
		size_t	found_pos;
		std::string	file("file=");
		found_pos = filename.find(file);
		if (found_pos == filename.npos)
			return -1;
		size_t	sep_pos = filename.find_first_of('&', found_pos);
		std::cerr << sep_pos << " " << found_pos <<'\n';
		if (sep_pos != filename.npos)
			sep_pos = sep_pos - (found_pos + file.length());
		filename = filename.substr(found_pos + file.length(), sep_pos);
		std::cerr << filename + '\n';
		return 0;
	}
	return -1;
}

int	get(fields& header, std::string& body)
{
	std::string	filename;
	if (find_filename(header, filename) < 0)
		return 404;
	if (access(filename.c_str(), F_OK | R_OK) != 0)
		return 404;
	std::ifstream is(filename.c_str());
	is >> body;
	return 0;
}

int	post(fields& header, std::string& server_body, std::string& body)
{
	std::string	filename;
	std::string	file_path;

	if (find_filename(header, filename) < 0)
		return 404;
	if (find_post_path(header, file_path) == 0)
	{
		if (chdir(file_path.c_str()) < 0)
			return -1;
	}
	std::ofstream	os(filename.c_str());
	std::cerr << filename << '\n';
	os << server_body;
	if (!os.is_open())
		std::cerr << "Not open\n";
	body = "Hi from the script!!!";
	return 0;
}

void	write_str(std::string const& str, int fd)
{
	write(fd, str.c_str(), str.length());
}

std::string	to_str(int n)
{
	std::stringstream	ss;
	ss << n;
	return ss.str();
}

int	main(int argc, char **argv, char **env)
{
	(void)argc;
	(void)argv;
	std::map<std::string, std::string>	fields;
	std::string	server_body;
	std::string	body;
	sleep(5);
	for (size_t i = 0; env[i]; ++i)
	{
		std::string temp = env[i];
		fields[temp.substr(0, temp.find_first_of('='))] = temp.substr(temp.find_first_of('=') + 1, temp.npos);
	}
	read_body(fields, server_body);
	std::string	method;
	int request_method = find_method(fields, method);
	int	status = 0;
	switch (request_method) {
		case GET :
			status = get(fields, body);
			break;
		case POST:
			status = post(fields, server_body, body);
	}
	if (status != 0)
		write_str("Status: 404 Not Found\r\n", STDOUT_FILENO);
	write_str("Content-Length: " + to_str(body.length()) + "\r\n\r\n", STDOUT_FILENO);
	write_str(body, STDOUT_FILENO);
}
