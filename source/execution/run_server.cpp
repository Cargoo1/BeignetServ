/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   run_server.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/29 19:10:40 by acamargo          #+#    #+#             */
/*   Updated: 2026/05/29 17:25:47 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include <configParser.hpp>
#include <cstddef>
#include <netinet/in.h>
# include <run_server.hpp>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <sys/poll.h>
#include <handle_request.hpp>
#include <Server.hpp>
#include <sys/socket.h>
#include <unistd.h>
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

int	close_all_clients(Server& server)
{
	if (server.getClients().size() <= 0)
		return 0;
	for (size_t i = 0; i < server.getClients().size(); i++)
		server.deleteClient(i);
	return 1;
}

void	close_servers(Server& server)
{
	std::vector<int>::iterator	it_sfds;
	std::vector<Client>::iterator	it_clients;

	for (it_sfds = server.getSfds().begin(); it_sfds != server.getSfds().end(); it_sfds++)
	{
		close(*it_sfds);
	}
	close_all_clients(server);
	close(server.getEpollfd());
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

bool	listen_msg(Client& c, int cfd)
{
	char	buff[BUFF_SIZE];

	std::memset(buff, 0, BUFF_SIZE);
	int		size_read = recv(cfd, &buff, BUFF_SIZE, 0);
	if (size_read <= 0)
		return false;
	c.appendMessage(buff);
	//std::cout << "READ {\n" << msg << "\n} END\n";
	return true;
}

void	accept_client(Server& server, int fd)
{
	struct sockaddr_in	client_info;
	long	client_ip;
	long	ip_parts[4];
	std::stringstream	ip;
	std::stringstream	port;
	socklen_t	client_info_len = sizeof(client_info);
	int new_fd = accept(fd,
						(struct sockaddr*)&client_info,
						&client_info_len);
	client_ip = ntohl(client_info.sin_addr.s_addr);
	ip_parts[0] = client_ip >> 24 & 255;
	ip_parts[1] = client_ip >> 16 & 255;
	ip_parts[2] = client_ip >> 8 & 255;
	ip_parts[3] = client_ip & 255;
	ip << ip_parts[0] << '.' << ip_parts[1] << '.' << ip_parts[2] << '.' << ip_parts[3];
	if (new_fd < 0)
	{
		std::cerr << strerror(errno);
		std::cerr << "\nCould not accept socket: ";
		std::cerr << fd << '\n';
		return;
	}
	client_info_len = sizeof(client_info);
	getsockname(new_fd, (struct sockaddr*)&client_info, &client_info_len);
	port << ntohs(client_info.sin_port);
	server.addClient(new_fd, EPOLLIN | EPOLLHUP, ip.str(), port.str());
}

void	get_client_msg(Server& server, int fd)
{
	if (!listen_msg(server.getClients().at(fd), fd))
	{
		std::cout << "Client: " << fd << " hang up, closing connection\n";
		server.deleteClient(fd);
		return;
	}
	handle_request(server.getClients().at(fd), server.getServerConf());
}

void	process_connection(Server&	server, int epollcount)
{
	for (int i = 0; i < epollcount; i++)
	{
		if (server.getEventQueue()[i].events & EPOLLHUP)
		{
			std::cout << "Client: " << server.getEventQueue()[i].data.fd << "hang up, closing connection\n";
			server.deleteClient(server.getEventQueue()[i].data.fd);
			continue;
		}
		if (server.getEventQueue()[i].data.fd <= server.getSfds().back())
			accept_client(server, server.getEventQueue()[i].data.fd);
		else
			get_client_msg(server, server.getEventQueue()[i].data.fd);
	}
}

#define MAX_EVENTS 10

int		set_epoll(Server& server)
{
	int		epollfd = -1;

	epollfd = epoll_create1(0);
	if (epollfd < 0)
	{
		std::cerr << "Epoll: " << strerror(errno) << '\n';
		return -1;
	}
	server.setEpollfd(epollfd);
	for (size_t i = 0; i < server.getSfds().size(); i++)
	{
		server.setEinf(server.getSfds().at(i), EPOLLIN);
		if (epoll_ctl(epollfd, EPOLL_CTL_ADD,
					server.getSfds().at(i),
					&server.getEinf()) < 0)
		{
			std::cerr << "Epoll: " << strerror(errno) << '\n';
			return -1;
		}
	}
	return 0;
}

int	run(std::vector<serverConfig> const& servers_conf)
{
	Server	server(servers_conf);
	std::vector<serverConfig>::const_iterator	it_svrconf;

	for (it_svrconf = servers_conf.begin(); it_svrconf != servers_conf.end(); it_svrconf++)
	{
		try
		{
			server.getSfds().push_back(getListenerSocket(it_svrconf->_serverName, it_svrconf->_listen));
		}
		catch (std::exception &e)
		{
			std::cerr << e.what() << '\n';
			return 1;
		}
	}
	if (set_epoll(server) < 0)
		return 1;
	while(true)
	{
		std::cout << "Waiting for events...\n";
		int epollcount = epoll_wait(server.getEpollfd(),
									server.getEventQueue(),
									MAX_EVENTS, 60000);
		if (epollcount < 0)
			return -1;
		if (epollcount == TIMEOUT)
		{
			std::cerr << "Timeout.\nClosing the connection with all the clients\n";
			if (!close_all_clients(server))
				break;
			continue;
		}
		else if (epollcount < 0)
		{
			close_servers(server);
			std::cerr << "Poll: " << strerror(errno) << '\n';
			return errno;
		}
		process_connection(server, epollcount);
	}
	return 1;
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
