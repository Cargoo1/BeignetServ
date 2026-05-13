/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/21 15:12:53 by acamargo          #+#    #+#             */
/*   Updated: 2026/05/13 19:30:34 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
	std::string msg = "GET      http://localhost:8080/ HTTP/1.1\r\nHost:localhost:aa\r\nContent-type:\r\n\r\n";
	send(sfd, msg.c_str(), msg.size(), 0);
	recv(sfd, buff, 100000, 0);
	std::cout << buff;
	sleep(1);
	listen(sfd, 10);
	//int new_fd = accept(sfd, NULL, NULL);
	//sleep(1000);
	//close(sfd);

	(void)argc;
	(void)argv;
	return EXIT_SUCCESS;
}
