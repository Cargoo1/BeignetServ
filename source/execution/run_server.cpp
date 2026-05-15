/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   run_server.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/29 19:10:40 by acamargo          #+#    #+#             */
/*   Updated: 2026/05/15 21:02:18 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "configClass/serverConfig.hpp"
#include "configParser.hpp"
#include <cstddef>
# include <run_server.hpp>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <sys/poll.h>
#include <handle_request.hpp>
#include <Server.hpp>
#include <sys/socket.h>
#include <vector>
#include <sys/epoll.h>
#define BUFF_SIZE 256

int		getListenerSocket(const std::string &host, const std::string &port)
{
	struct addrinfo	hints;
	struct addrinfo	*result;
	struct addrinfo	*temp;
	int		gai_errno;
	int		sfd = -1;
	const int		enable = 1;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_protocol = 0;
	hints.ai_socktype = SOCK_STREAM;
	if ((gai_errno = getaddrinfo(host.c_str(), port.c_str(), &hints, &result)) != 0)
		throw std::runtime_error("gai: " + std::string(gai_strerror(gai_errno)));
	for (temp = result; temp != NULL; temp = temp->ai_next)
	{
		sfd = socket(temp->ai_family, temp->ai_socktype, temp->ai_protocol);
		if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
			throw std::runtime_error(strerror(errno));
		if (sfd < 0)
			continue;
		if (bind(sfd, temp->ai_addr, temp->ai_addrlen) < 0)
			continue;
		break;
	}
	if (temp == NULL)
	{
		freeaddrinfo(result);
		throw std::runtime_error(
			"Could not bind() or socket(): "
			+ port + ' '
			+ std::string(strerror(errno)));
	}
	freeaddrinfo(result);
	if (listen(sfd, 10) < 0)
		throw std::runtime_error("Listen: " + std::string(strerror(errno)));
	return sfd;
}

void	close_servers(std::vector<Server>& servers)
{
	std::vector<Server>::iterator	it_servers;
	std::vector<struct pollfd>::iterator	it_pfds;

	for (it_servers = servers.begin(); it_servers != servers.end(); it_servers++)
	{
		for (it_pfds = it_servers->getPfd().begin(); it_pfds != it_servers->getPfd().end(); it_pfds++)
		{
			close(it_pfds->fd);
		}
	}
}
/*
void	add_client(int pfd, std::vector<struct pollfd> &pfds, std::vector<Server> &clients)
{
	struct pollfd temp;
	temp.events = POLLIN;
	temp.fd = pfd;
	temp.revents = 0;
	pfds.push_back(temp);
	clients.push_back(Server(pfd, POLLIN));
}
*/

bool	listen_msg(std::string& msg, int pfd)
{
	char	buff[BUFF_SIZE];

	memset(buff, 0, BUFF_SIZE);
	int		size_read = recv(pfd, &buff, BUFF_SIZE, 0);
	if (size_read <= 0)
		return false;
	msg.append(buff, BUFF_SIZE);
	std::cout << "READ {\n" << msg << "\n} END\n";
	return true;
}

void	process_connection(Server&	server)
{
	int		new_fd;

	for (size_t i = 0; i < server.getPfd().size(); i++)
	{
		if (!((server.getPfd().at(i)).revents & (POLLIN | POLLHUP)))
			continue;
		if ((server.getPfd().at(i)).fd == server.getSfd())
		{
			new_fd = accept(server.getPfd().at(i).fd, NULL, NULL);
			if (new_fd < 0)
			{
				std::cerr << strerror(errno);
				std::cerr << "\nCould not accept socket: ";
				std::cerr << server.getPfd().at(i).fd << '\n';
			}
			else
			{
				server.addClient(new_fd);
				std::cout << "New client connected: " << new_fd << '\n';
			}
		}
		else
		{
			if (!listen_msg(server.getClients().at(i - 1).getMessage(), server.getPfd().at(i).fd))
			{
				std::cerr << "Client: " << server.getPfd().at(i).fd << " hung up\n";
				close(server.getPfd().at(i).fd);
				server.getPfd().erase(server.getPfd().begin() + i);
				server.getClients().erase(server.getClients().begin() + (i - 1));
			}
			else if (server.getClients().at(i - 1).getMessage().find("\r\n\r\n")!= std::string::npos)
			{
				handle_request(server.getClients().at(i - 1).getMessage());
			}
		}
	}
}

#define MAX_EVENTS 10
int		set_epoll(Server const& server)
{
	int		epollfd = -1;

	epollfd = epoll_create1(0);
	if (epollfd < 0)
	{
		std::cerr << "Epoll: " << strerror(errno) << '\n';
		return -1;
	}
	for (size_t i = 0; i < sfds.size(); i++)
	{
		sfds_inf[i].data.fd = sfds.at(i);
		sfds_inf[i].events = EPOLLIN;
		if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sfds.at(i), &sfds_inf[i]) < 0)
		{
			std::cerr << "Epoll: " << strerror(errno) << '\n';
			return -1;
		}
	}
	return epollfd;

}

int	run(std::vector<serverConfig> const& servers_conf)
{
	Server	server(servers_conf);
	std::vector<serverConfig>::const_iterator	it_svrconf;

	for (it_svrconf = servers_conf.begin(); it_svrconf != servers_conf.end(); it_svrconf++)
	{
		try
		{
			server.getSfds().push_back(getListenerSocket(it_svrconf->_serverName, it_svrconf->_port));
		}
		catch (std::exception &e)
		{
			std::cerr << e.what() << '\n';
			return -1;
		}
	}
	for (std::vector<int>::iterator it = server.getSfds().begin(); it != server.getSfds().end(); it++)
	{
		server.addE_sfds_inf(*it, EPOLLIN);
	}
	server.setEpollfd(set_epoll())
	if (epollfd < 0)
	{
		delete sfds_inf;
	}
	std::vector<Server>::iterator	it_servers;
	std::cout << "Waiting for connections...\n";
	for(;;)
	{
		for (it_servers = sfds.begin(); it_servers != sfds.end(); it_servers++)
		{

			int	pollcount = poll(it_servers->getPfd().data(), it_servers->getPfd().size(), 60000);
			if (pollcount == TIMEOUT)
			{
				close_servers(sfds);
				std::cerr << "Timeout.\nEnding the comunication\n";
				return TIMEOUT;
			}
			else if (pollcount < 0)
			{
				close_servers(sfds);
				std::cerr << "Poll: " << strerror(errno) << '\n';
				return errno;
			}
			process_connection(*it_servers);
		}
	}
}
/*
int	main(void)
{
	std::vector<int>ports;
	for (int i = 0; i < 10; i++)
	{
		ports.push_back(i);
	}
	std::string name  = "a";
	try {
		run("localhost", "8888");
	}
	catch(std::exception &e) {
		std::cout << e.what() << "\n";
	}
	return 0;
}
*/
