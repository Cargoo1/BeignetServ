/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/21 15:12:53 by acamargo          #+#    #+#             */
/*   Updated: 2026/07/17 15:03:18 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <climits>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <cstdlib>
#include <map>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>

int	main(int argc, char **argv)
{
	// if (argc != 2)
	// {
	// 	std::cerr << "Error: args" << std::endl;
	// 	return (EXIT_FAILURE);
	// }
	std::map<std::string, std::string> test;
	test["lol"] = "so";
	std::cout << test["lol"];
	struct addrinfo addrinfo;
	memset(&addrinfo, 0, sizeof addrinfo);
	struct addrinfo *result;
	addrinfo.ai_family = AF_INET;
	addrinfo.ai_socktype = SOCK_STREAM;
	addrinfo.ai_protocol = 0;
	int error = getaddrinfo("localhost", "8080", &addrinfo, &result);
	// int error = getaddrinfo("localhost", "8888", &addrinfo, &result);
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
	char buff[100000];
	std::string msg = "POST /test.py HTTP/1.1\r\nHost:localhost:8080\r\nTransfer-Encoding:chunked\r\n\r\n";
	// int bytes_sent = send(sfd, msg.c_str(), msg.length(), 0);
	send(sfd, msg.c_str(), msg.length(), 0);
	// sleep(5);
	send(sfd, "B", 1, 0);
	//sleep(5);
	send(sfd, "\r\n", 2, 0);
	//sleep(5);
	msg = "hello world\r\n";
	send(sfd, msg.c_str(), msg.length(), 0);
	//sleep(5);
	send(sfd, "0\r\n", 3, 0);
	send(sfd, "\r\n", 2, 0);
	memset(buff, 0, sizeof(buff));
	recv(sfd, buff, 100000, 0);
	std::cout << buff;
	//listen(sfd, 10);
	//int new_fd = accept(sfd, NULL, NULL);
	//sleep(1000);
	//close(sfd);

	(void)argc;
	(void)argv;
	return EXIT_SUCCESS;
}
