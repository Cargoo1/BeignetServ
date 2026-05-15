/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 15:08:39 by acamargo          #+#    #+#             */
/*   Updated: 2026/05/15 20:59:42 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Request.hpp"
#include "configClass/serverConfig.hpp"
#include <sys/epoll.h>
#include <sys/poll.h>
#include <vector>
#include <Client.hpp>

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
	void						addE_sfds_inf(int pfd, uint32_t events);
private:
	std::vector<int>			_sfds;
	int							_epollfd;
	std::vector<serverConfig> const&	_server_conf;
	std::vector<struct epoll_event>	_e_sfds_inf;
};
