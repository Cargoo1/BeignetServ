/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   run_server.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/29 19:10:40 by acamargo          #+#    #+#             */
/*   Updated: 2026/07/21 15:39:10 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Request.hpp"
#include "utils.hpp"
#include "utils_logs.hpp"
#include <configParser.hpp>
#include <cstddef>
#include <cstdlib>
#include <ctime>
#include <map>
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
#include <signal.h>
#include <sys/epoll.h>

volatile bool stopExec = false;

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

void	check_idle_clients(Server& server)
{
	time_t	curr_time;

	std::time(&curr_time);
	if (curr_time - server.getLastCheck() < 1)
		return;
	std::map<int, Client>::iterator	it;
	for (it = server.getClients().begin(); it != server.getClients().end();)
	{
		if (curr_time - it->second.getLastComm() < 160)
		{
			++it;
			continue;
		}
		print_log(TEXT_YELLOW, NULL, "Removing iddle client: "
								+ it->second.getIp() + ":"
								+ it->second.getPort() + ", fd: "
								+ toStr(it->first), 0);
		if (it->second.getRequest().getPipeFd() != -1)
			server.deleteCgiChild(it->second.getRequest().getPipeFd());
		server.deleteClient((it++)->first);
	}
	server.setLastCheck();
}

void	get_client_request(Server& server, int fd)
{
	Client&	client = server.getClients().at(fd);
	int		infno = 0;

	if (recv_msg(client.getNotConstMsg(), fd) != 0)
	{
		server.deleteClient(fd);
		return;
	}
	client.setLastComm();
	while (!client.getMsg().empty())
	{
		infno = handle_request(server.getClients().at(fd), server);
		if (infno != DONE)
			break;
	}
	if (infno == RUN_CGI)
		server.addCgiChild(EPOLLIN, server.getClients().at(fd));
}

void	process_data(Server&	server, int epollcount)
{
	for (int i = 0; i < epollcount; i++)
	{
		if (server.getCgiChilds().find(server.getEventQueue()[i].data.fd)
			!= server.getCgiChilds().end())
		{
			if (get_script_output(server.getCgiChilds().at(server.getEventQueue()[i].data.fd)) == 0)
				server.deleteCgiChild(server.getEventQueue()[i].data.fd);
			continue;
		}
		if (server.getEventQueue()[i].events & EPOLLHUP)
		{
			server.deleteClient(server.getEventQueue()[i].data.fd);
			continue;
		}
		if (server.getEventQueue()[i].data.fd <= server.getSfds().back())
			accept_client(server, server.getEventQueue()[i].data.fd);
		else
			get_client_request(server, server.getEventQueue()[i].data.fd);
	}
	check_idle_clients(server);
	check_idle_scripts(server);
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
			server.getSfds().push_back(getListenerSocket(it_svrconf->serverName(), it_svrconf->listen()));
		}
		catch (std::exception &e)
		{
			std::cerr << e.what() << '\n';
			return 1;
		}
	}
	if (set_epoll(server) < 0)
		return 1;
	print_log(TEXT_BLUE, NULL, "Waiting connections...", 0);
	signal(SIGINT, ft_handler);
	while(stopExec == false)
	{
		int epollcount = epoll_wait(server.getEpollfd(),
									server.getEventQueue(),
									MAX_EVENTS, 1000);
		if (epollcount < 0)
			return -1;
		if (epollcount == TIMEOUT)
		{
			check_idle_clients(server);
			continue;
		}
		else if (epollcount < 0)
		{
			close_servers(server);
			print_log(TEXT_RED, NULL, std::string("Poll: ") + strerror(errno), 1);
			return errno;
		}
		process_data(server, epollcount);
	}
	return 1;
}
