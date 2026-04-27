/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/21 15:12:53 by acamargo          #+#    #+#             */
/*   Updated: 2026/04/27 15:56:35 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstdio>
#include <cstring>
#include <iostream>
#include <cstdlib>
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
	struct addrinfo addrinfo;
	memset(&addrinfo, 0, sizeof addrinfo);
	struct addrinfo *result;
	addrinfo.ai_family = AF_INET;
	addrinfo.ai_socktype = SOCK_STREAM;
	addrinfo.ai_protocol = 0;
	int error = getaddrinfo("localhost", "8888", &addrinfo, &result);
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
	send(sfd, "hola", 4, 0);
	sleep(1000);
	close(sfd);

	(void)argc;
	(void)argv;
	return EXIT_SUCCESS;
}
