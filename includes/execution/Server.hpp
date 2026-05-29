/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 15:08:39 by acamargo          #+#    #+#             */
/*   Updated: 2026/05/29 17:12:03 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Request.hpp"
#include <cstddef>
#include <serverConfig.hpp>
#include <map>
#include <sys/epoll.h>
#include <sys/poll.h>
#include <vector>
#include <Client.hpp>
#define MAX_EVENTS 10

class	Server
{
public:
	Server(Server const& other);
	Server(std::vector<serverConfig> const& servers_conf);
	~Server();

	Server& operator=(Server const& other);

	std::vector<int>&			getSfds(void);
	int							getEpollfd(void);
	void						setEpollfd(int fd);
	struct epoll_event&	getEinf(void);
	std::vector<serverConfig> const&	getServerConf(void) const;
	void						setEinf(int fd, uint32_t events);
	struct epoll_event*			getEventQueue(void);
	std::map<int, Client>&		getClients(void);
	void						addClient(int fd, uint32_t events, std::string const& ip, std::string const& port);
	void						deleteClient(int fd);
private:
	std::vector<int>			_sfds;
	int							_epollfd;
	std::vector<serverConfig> const&	_server_conf;
	struct epoll_event			_einf, _eventQueue[MAX_EVENTS];
	std::map<int, Client>		_clients;
};
