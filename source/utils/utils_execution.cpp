/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_execution.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/22 20:16:17 by acamargo          #+#    #+#             */
/*   Updated: 2026/07/23 22:56:37 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
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

