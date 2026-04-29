/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   run_server.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/29 19:10:40 by acamargo          #+#    #+#             */
/*   Updated: 2026/04/29 21:14:40 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "includes/run_server.hpp"
#include <cerrno>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <map>
#include <netdb.h>
#include <stdexcept>
#include <sys/poll.h>
#include <sys/socket.h>
#include <vector>

int		getListenerSocket(const std::map<std::string, std::string> &server)
{
	struct addrinfo	hints;
	struct addrinfo	*result;
	struct addrinfo	*temp;
	int		gai_errno;
	int		sfd = -1;

	hints.ai_family = AF_INET;
	hints.ai_protocol = 0;
	hints.ai_socktype = SOCK_STREAM;
	if ((gai_errno = getaddrinfo(server.at("interface").c_str(), server.at("port").c_str(), &hints, &result)) != 0)
		throw std::runtime_error(gai_strerror(gai_errno));
	for (temp = result; temp != NULL; temp = temp->ai_next)
	{
		sfd = socket(temp->ai_family, temp->ai_socktype, temp->ai_protocol);
		if (sfd < 0)
			continue;
		if (bind(sfd, temp->ai_addr, temp->ai_addrlen) < 0)
			continue;
		break;
	}
	if (temp == NULL)
	{
		freeaddrinfo(result);
		throw std::runtime_error(strerror(errno));
	}
	freeaddrinfo(result);
	if (listen(sfd, 10) < 0)
		throw std::runtime_error(strerror(errno));
	return sfd;
}

void	run(Config &c)
{
	const servers_list &s = c.getServers();
	servers_list::const_iterator it;
	std::vector<struct pollfd>	pfds;
	int		sfd;

	sfd = getListenerSocket(*it);
	for(;;)
	{
		int	pollcount = poll(pfds, 1, 10000);
	}
}

int	main(void)
{
	std::vector<int>ports;
	for (int i = 0; i < 10; i++)
	{
		ports.push_back(i);
	}
	std::string name  = "a";
	try {
		run(ports, name);
	}
	catch(std::exception &e) {
		std::cout << e.what();
	}
	return 0;
}
