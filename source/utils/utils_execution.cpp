/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_execution.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/22 20:16:17 by acamargo          #+#    #+#             */
/*   Updated: 2026/08/03 18:06:41 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Header.hpp"
#include <ctime>
#include <map>
#include <unistd.h>
#include <utils_execution.hpp>

int	writeall(int fd, char const* buf, size_t &len)
{
	int	bytes_sent = 0;
	int	bytes_left = len;
	int	total_bytes_sent = 0;

	while (total_bytes_sent < static_cast<int>(len))
	{
		bytes_sent = write(fd, buf + total_bytes_sent, bytes_left);
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

int	sendall(int fd, char const* buf, size_t &len)
{
	int	bytes_sent = 0;
	int	bytes_left = len;
	int	total_bytes_sent = 0;

	while (total_bytes_sent < static_cast<int>(len))
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

void	close_pipe(int* pipe)
{
	if (pipe[0] > 0)
		close(pipe[0]);
	if (pipe[1] > 0)
		close(pipe[1]);
	pipe[0] = -1;
	pipe[1] = -1;
}

void generateTimestampFilename(std::string& filename)
{
	std::time_t resultTime = std::time(0);
	std::string timestamp = std::asctime(std::localtime(&resultTime));
	timestamp.erase(timestamp.length() - 1, 1);
	if (filename.empty()) {
		filename = timestamp;
		return;
	}
	std::string tmp;
	std::string::size_type pos = filename.find_last_of(".");
	if (pos == 0 || pos == std::string::npos) {
		filename += '-' + timestamp;
	}
	tmp = filename.substr(0, pos);
	std::string newFileName = tmp + "-" + timestamp;
	tmp = filename.substr(pos);
	newFileName += tmp;
	filename = newFileName;
}

int		find_filename(Header const& header, std::string& filename)
{
	if (!header.getFilename().empty())
	{
		filename = header.getFilename();
		return 0;
	}
	if (!header.getQueryStr().empty())
	{
		std::string query_str = header.getQueryStr();
		std::string	file_var("file=");
		size_t	file_var_pos = query_str.find(file_var);
		if (file_var_pos != query_str.npos)
		{
			size_t	separator_pos = query_str.find_first_of('&', file_var_pos + file_var.length());
			if (separator_pos != query_str.npos)
				separator_pos = separator_pos - (file_var_pos + file_var.length());
			filename = query_str.substr(file_var_pos + file_var.length(), separator_pos);
			return 0;
		}
	}
	if (header.getTargetResource().at(header.getTargetResource().length() - 1) != '/')
	{
		size_t	last_slash_pos = header.getTargetResource().find_last_of('/');
		filename = header.getTargetResource().substr(last_slash_pos + 1, std::string::npos);
		return 0;
	}
	generateTimestampFilename(filename);
	return 0;
}
