/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   run_server.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/29 19:10:40 by acamargo          #+#    #+#             */
/*   Updated: 2026/07/25 01:12:21 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiChild.hpp"
#include "Client.hpp"
#include "Request.hpp"
#include "send_http_response.hpp"
#include "utils.hpp"
#include "utils_execution.hpp"
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
#include <set>
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
#include <sys/wait.h>

volatile bool stop_server = false;

namespace {
	void check_child(std::map<int, CgiChild> &childMap, Server &serv) {
		std::map<int, CgiChild>::iterator it = childMap.begin();
		for (; it != childMap.end(); it++) {
			if (it->second.is_done){
				time_t curr;
				std::time(&curr);
				if (curr - it->second.getLastComm() >= 20)
					serv.removeChild(it->second);
			}
		}
	}
}

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
		{
			close(sfd);
			throw std::runtime_error(strerror(errno));
		}
		if (sfd < 0)
			continue;
		if (bind(sfd, temp->ai_addr, temp->ai_addrlen) < 0)
		{
			close(sfd);
			continue;
		}
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
	server.addClient(new_fd, EPOLLIN, ip.str(), port.str());
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
		server.deleteClient((it++)->first, true);
	}
	server.setLastCheck();
}

void	get_client_request(Server& server, int fd)
{
	Client&	client = server.getClients().at(fd);
	int		infno = 0;

	if (client.getRequest().is_cgi_in_progress)
		return ;
	if (recv_msg(client.getNotConstMsg(), fd) != 0)
	{
		if (client.getRequest().getPipeFd() != -1)
			server.deleteCgiChild(client.getRequest().getPipeFd());
		server.deleteClient(fd, true);
		return;
	}
	client.setLastComm();
	handle_request(server.getClients().at(fd), server);
	return;



	while (!client.getMsg().empty())
	{
		infno = handle_request(server.getClients().at(fd), server);
		if (infno != DONE)
			break;
	}
}

void	handle_event(Server& server, uint32_t events, int fd)
{
	int	epollin = events & EPOLLIN;
	int	epollout = events & EPOLLOUT;
	int	epollup = events & EPOLLHUP;

	if (server.getClients().find(fd) != server.getClients().end())
	{
		Client&	client = server.getClients().at(fd);
		if (epollin && !client.getRequest().failed_request
			&& !client.getRequest().is_request_done)
			get_client_request(server, fd);
		if (epollout && (client.getRequest().failed_request || client.getRequest().is_request_done))
		{
			send_response(client.getRequest(), client.getFd());
			server.set_2_epoll(EPOLLIN, fd, EPOLL_CTL_MOD);
			if (client.getRequest().failed_request)
				server.deleteClient(fd, true);
		}
	}
	else if (server.getCgiChilds().find(fd) != server.getCgiChilds().end())
	{
		CgiChild&	child = server.getCgiChilds().at(fd);
		if (!child.is_script_running)
			return;
		if (epollin)
			get_script_output(server, fd);
		if (epollout)
			send_response(child.getClientOwner().getRequest(), child.getClientOwner().getFd());
		if (epollup)
		{
			child.getClientOwner().getRequest().failed_request = true;
			child.getClientOwner().getRequest().getResponse().setStatusCode(bad_geteway);
			server.remove_from_epoll(fd);
			close_pipe(child.getOutputPipe());
		}
	}
	else if (server.getInputPipes().find(fd) != server.getInputPipes().end())
	{
		CgiChild&	child = *server.getInputPipes().at(fd);
		if (epollout && !child.is_script_running)
		{
			child.execute_script();
			server.remove_from_epoll(fd);
			close_pipe(child.getInputPipe());
		}
	}
	return;
}

void	process_data(Server&	server, int epollcount)
{
	int		fd = 0;
	for (int i = 0; i < epollcount; i++)
	{
		fd = server.getEventQueue()[i].data.fd;
		if (server.getEventQueue()[i].events & EPOLLHUP
			&& server.getCgiChilds().find(fd) == server.getCgiChilds().end())
			server.remove_from_epoll(fd);
		else if (fd <= server.getSfds().back() && server.getEventQueue()[i].events & EPOLLIN)
			accept_client(server, fd);
		else
			handle_event(server, server.getEventQueue()[i].events, fd);
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
			server.close_server_sockets();
			return 1;
		}
	}
	if (set_epoll(server) < 0)
		return 1;
	print_log(TEXT_BLUE, NULL, "Waiting connections...", 0);
	signal(SIGINT, ft_handler);
	while(!stop_server)
	{
		print_log(TEXT_MAGENTA, NULL, "Waiting epoll....", 0);
		int epollcount = epoll_wait(server.getEpollfd(),
									server.getEventQueue(),
									MAX_EVENTS, 1000);
		if (epollcount == TIMEOUT)
		{
			check_idle_clients(server);
			check_child(server.getCgiChilds(), server);
			continue;
		}
		else if (epollcount < 0)
		{
			server.close_server(true);
			print_log(TEXT_RED, NULL, std::string("Epoll: ") + strerror(errno), 1);
			return errno;
		}
		process_data(server, epollcount);
	}
	server.close_server(true);
	return 1;
}
