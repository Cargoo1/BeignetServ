/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/27 15:25:36 by acamargo          #+#    #+#             */
/*   Updated: 2026/04/27 15:58:15 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstring>
#include <netdb.h>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>


int	main(void)
{
	struct addrinfo addrinfo;
	struct addrinfo	*result;
	memset(&addrinfo, 0, sizeof addrinfo);
	addrinfo.ai_family = AF_INET;
	addrinfo.ai_protocol = 0;
	addrinfo.ai_socktype = SOCK_STREAM;
	addrinfo.ai_flags = 0;
	if (getaddrinfo("localhost", "8888", &addrinfo, &result))
	{
		perror("getaddrinfo");
		exit(1);
	}
	int sfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (bind(sfd, result->ai_addr, result->ai_addrlen) != 0)
	{
		perror("bind");
		exit(1);
	}
	if (listen(sfd, 128) != 0)
	{
		perror("listen");
		exit(1);
	}
	int client = accept(sfd, NULL, NULL);
	char	buff[4096];
	while (1)
	{
		int recived = recv(client, buff, sizeof buff, 0);
		printf("???\n");
		if (recived < 0)
		{
			perror("recv");
			exit(1);
		}
		if (!recived)
			break;
		printf("%s\n", buff);
	}
}
