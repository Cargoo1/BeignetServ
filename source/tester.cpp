/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tester.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 23:13:46 by acamargo          #+#    #+#             */
/*   Updated: 2026/07/28 23:11:30 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstddef>
#include <fstream>
#include <ios>
#include <iostream>
#include <map>
#include <unistd.h>
int	main(int argc, char **argv, char **env)
{
	(void)argc;
	(void)argv;
	while (true)
	{
		;
	}
	std::map<std::string, std::string>	fields;
	std::string	temp;
	for (size_t i = 0; env[i]; ++i)
	{
		temp = env[i];
		std::cerr << env[i] << '\n';
		fields[temp.substr(0, temp.find_first_of('='))] = temp.substr(temp.find_first_of('=') + 1, temp.npos);
	}
	std::map<std::string, std::string>::iterator it = fields.find("CONTENT_LENGTH");
	if (it == fields.end())
		return 1;
	char	buff[250];
	std::cerr << "???\n";
	size_t bytes_read = read(0, &buff, 249);
	std::cerr << "!!\n";
	std::cerr << "\r\n";
	buff[bytes_read] = '\0';
	std::string	input = buff;
	it = fields.find("QUERY_STRING");
	std::string var("name=");
	size_t	var_pos = it->second.find(var);
	std::string	msg;
	if (var_pos == std::string::npos)
	{
		std::cerr << "Not found\n";
		msg = "Status: 404 Not found\n";
		write(1, msg.c_str(), msg.length());
		return 0;
	}
	std::string	filename = it->second.substr(var_pos + var.length(), filename.npos);
	std::cerr << filename << '\n';
	std::ofstream	fstream(filename.c_str());
	fstream << input;
	std::cerr << "sending\n";
	msg = "Status: 200 OK\r\nContent-length:    0\r\n\r\n";
	write(1, msg.c_str(), msg.length());
	close(STDOUT_FILENO);
}
