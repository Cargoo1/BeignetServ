/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   run_server.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/29 19:10:40 by acamargo          #+#    #+#             */
/*   Updated: 2026/04/30 21:34:28 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../includes/run_server.hpp"
#include <cerrno>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <sys/poll.h>
#include <sys/socket.h>
#include <vector>

int		getListenerSocket(const std::string &host, const std::string &port)
{
	struct addrinfo	hints;
	struct addrinfo	*result;
	struct addrinfo	*temp;
	int		gai_errno;
	int		sfd = -1;

	hints.ai_family = AF_INET;
	hints.ai_protocol = 0;
	hints.ai_socktype = SOCK_STREAM;
	memset(&hints, 0, sizeof hints);
	if ((gai_errno = getaddrinfo(host.c_str(), port.c_str(), &hints, &result)) != 0)
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

void	close_server(const std::vector<int> &sfds, const std::vector<struct pollfd> &pfds)
{
	std::vector<int>::const_iterator	it;
	std::vector<struct pollfd>::const_iterator	it_pfd;
	for (it = sfds.begin(); it != sfds.end(); it++)
		close(*it);
	for (it_pfd = pfds.begin(); it_pfd != pfds.end(); it_pfd++)
		close((*it_pfd).fd);
}

void	add_pfds(int pfd, std::vector<struct pollfd> &pfds)
{
	struct pollfd temp;
	temp.events = POLLIN;
	temp.fd = pfd;
	temp.revents = 0;
	pfds.push_back(temp);
}

void	process_connection(int sfd, std::vector<struct pollfd> &pfds)
{
	std::vector<struct pollfd>::iterator it;
	char	buff[1000];
	int		new_fd;

	memset(&buff, 0, sizeof buff);
	for (size_t i = 0; i < pfds.size(); i++)
	{
		if ((pfds.at(i)).revents & (POLLIN | POLLHUP))
		{
			if ((pfds.at(i)).fd == sfd)
			{
				new_fd = accept(sfd, NULL, NULL);
				if (new_fd < 0)
					std::cerr << strerror(errno);
				else
				{
					add_pfds(new_fd, pfds);
				}
			}
			else
			{
				int		size_read = recv((pfds.at(i)).fd, &buff, 1000, 0);
				if (size_read <= 0)
				{
					if (size_read == 0)
						std::cout << "BeignetServ: socket " << (pfds.at(i)).fd << " hung up\n";
					close((pfds.at(i)).fd);
					pfds.erase(pfds.begin()+i);
					continue;
				}
				std::cout << "socket: " << (pfds.at(i)).fd << ": " << buff;
			}
		}
	}
}

void	run(const std::string &host, const std::string &port)
{
	std::vector<struct pollfd>	pfds;
	std::vector<int>			sfd;

	sfd.push_back(getListenerSocket(host, port));
	struct pollfd test;
	test.fd = sfd.front();
	test.events = POLLIN;
	test.revents = 0;
	pfds.push_back(test);
	for(;;)
	{
		std::cout << "Waiting for connections...\n";
		int	pollcount = poll(pfds.data(), pfds.size(), 5000);
		if (pollcount == TIMEOUT)
		{
			close_server(sfd, pfds);
			throw std::runtime_error("Timeout.\nEnding the comunication\n");
		}
		else if (pollcount < 0)
		{
			close_server(sfd, pfds);
			throw std::runtime_error(strerror(errno));
		}
		process_connection(sfd.front(), pfds);
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
		run("localhost", "6666");
	}
	catch(std::exception &e) {
		std::cout << e.what() << "\n";
	}
	return 0;
}
