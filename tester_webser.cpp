/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/21 15:12:53 by acamargo          #+#    #+#             */
/*   Updated: 2026/07/28 23:55:14 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cerrno>
#include <climits>
#include <csignal>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <cstdlib>
#include <map>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <vector>

char	buff[2000];

void	recv_response(int sfd)
{
	memset(&buff, 0, sizeof(buff));
	int bytes_recv = recv(sfd, buff, sizeof(buff) - 1, 0);
	if (bytes_recv < 0)
	{
		std::cerr << strerror(errno) << '\n';
		return;
	}
	buff[bytes_recv] = '\0';
	std::cout << buff << "<<<\n\n";
}

void	send_str(int sfd, std::string const& str)
{
	send(sfd, str.c_str(), str.length(), 0);
}

void	test_get(std::string const& uri, std::vector<std::string>& vector, std::string const& body, int sfd)
{
	std::string	msg;
	std::vector<std::string>::iterator	it = vector.begin();

	msg = "GET " + uri + " HTTP/1.1\r\n";
	for (; it != vector.end(); ++it)
		msg.append(*it + "\r\n");
	msg.append("\r\n" + body);
	send(sfd, msg.c_str(), msg.length(), 0);
}

void	test_post(std::string const& uri, std::vector<std::string>& vector, std::string const& body, int sfd)
{
	std::string	msg;
	std::vector<std::string>::iterator	it = vector.begin();

	msg = "POST " + uri + " HTTP/1.1\r\n";
	for (; it != vector.end(); ++it)
		msg.append(*it + "\r\n");
	msg.append("\r\n" + body);
	send(sfd, msg.c_str(), msg.length(), 0);
}
int	main()
{
	struct addrinfo addrinfo;
	memset(&addrinfo, 0, sizeof addrinfo);
	struct addrinfo *result;
	addrinfo.ai_family = AF_INET;
	addrinfo.ai_socktype = SOCK_STREAM;
	addrinfo.ai_protocol = 0;
	int error = getaddrinfo("localhost", "8080", &addrinfo, &result);
	if (error != 0)
	{
		perror("?\n");
		exit(error);
	}
	int sfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (connect(sfd, result->ai_addr, result->ai_addrlen) != 0)
	{
		perror("connect");
		exit(1);
	}
	{
		std::vector<std::string>	header;

		header.push_back("Host:localhost:8080");
		test_get("/", header, "", sfd);
		recv_response(sfd);
	}
	{
		std::vector<std::string>	header;
		header.push_back("Host:localhost:8080");
		header.push_back("Transfer-Encoding:chunked");
		test_post("/testing", header, "", sfd);
		send_str(sfd, "1\r\n");
		sleep(2);
		send_str(sfd, "a\r\nA\r\n");
		sleep(2);
		send_str(sfd, "0123456789\r\n0\r\n");
		sleep(5);
		send_str(sfd, "\r\n");
		recv_response(sfd);
	}
	{
		std::vector<std::string>	header;
		header.push_back("Host:localhost:8080");
		header.push_back("Transfer-Encoding:chunked");
		test_post("/testing", header, "", sfd);
		send_str(sfd, "1\r\n");
		sleep(2);
		send_str(sfd, "?\r\nA\r\n");
		sleep(2);
		send_str(sfd, "0123456789\r\n0\r\n");
		sleep(5);
		send_str(sfd, "Sasdasd");
		send_str(sfd, "\r\n");
		recv_response(sfd);
		if (connect(sfd, result->ai_addr, result->ai_addrlen) != 0)
		{
			perror("connect");
			exit(1);
		}
	}
	return EXIT_SUCCESS;
}
