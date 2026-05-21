/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   run_server.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/29 19:10:40 by acamargo          #+#    #+#             */
/*   Updated: 2026/05/21 22:34:18 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <configParser.hpp>
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

void	close_servers(Server& servers)
{
	std::vector<int>::iterator	it_sfds;
	std::vector<Client>::iterator	it_clients;

	for (it_sfds = servers.getSfds().begin(); it_sfds != servers.getSfds().end(); it_sfds++)
	{
		close(*it_sfds);
	}
	for (it_clients = servers.getClients().begin(); it_clients != servers.getClients().end(); it_clients++)
	{
		close(it_clients->getFd());
	}
	close(servers.getEpollfd());
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

void	process_connection(Server&	server, int epollcount)
{
	int		new_fd;

	for (int i = 0; i < epollcount; i++)
	{
		if (server.getEventQueue()[i].data.fd <= server.getSfds().back())
		{
			new_fd = accept(server.getEventQueue()[i].data.fd, NULL, NULL);
			if (new_fd < 0)
			{
				std::cerr << strerror(errno);
				std::cerr << "\nCould not accept socket: ";
				std::cerr << server.getEventQueue()[i].data.fd << '\n';
			}
			else
			{
				server.addClient(new_fd, EPOLLIN);
				std::cout << "New client connected from: " << new_fd << '\n';
			}
		}
		else
		{
			size_t	client_index = server.getClients().front().getFd() - server.getEventQueue()[i].data.fd;
			if (!listen_msg(server.getClients().at(client_index).getMessage(),
							server.getEventQueue()[i].data.fd))
			{
				std::cerr << "Client: " << server.getClients().at(client_index).getFd() << " hung up\n";
				close(server.getClients().at(client_index).getFd());
				server.getClients().erase(server.getClients().begin() + client_index);
				continue;
			}
			handle_request(server.getClients().at(client_index), server.getServerConf());
		}
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
			server.getSfds().push_back(getListenerSocket(it_svrconf->_serverName, it_svrconf->_port));
		}
		catch (std::exception &e)
		{
			std::cerr << e.what() << '\n';
			return -1;
		}
	}
	if (set_epoll(server) < 0)
		return -1;
	std::cout << "Waiting for connections...\n";
	for(;;)
	{

		int epollcount = epoll_wait(server.getEpollfd(),
									server.getEventQueue(),
									MAX_EVENTS, 30000);
		if (epollcount < 0)
			return -1;
		if (epollcount == TIMEOUT)
		{
			close_servers(server);
			std::cerr << "Timeout.\nEnding the comunication\n";
			return TIMEOUT;
		}
		else if (epollcount < 0)
		{
			close_servers(server);
			std::cerr << "Poll: " << strerror(errno) << '\n';
			return errno;
		}
		process_connection(server, epollcount);
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
