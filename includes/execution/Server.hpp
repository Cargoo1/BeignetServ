/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 15:08:39 by acamargo          #+#    #+#             */
/*   Updated: 2026/07/21 13:07:08 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "CgiChild.hpp"
#include "Request.hpp"
#include "SessionManager.hpp"
#include <cstddef>
#include <ctime>
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
	time_t						getLastCheck(void) const;
	void						setLastCheck(void);
	time_t						getLastCheckScripts(void) const;
	void						setLastCheckScripts(void);
	void						addClient(int fd, uint32_t events, std::string const& ip, std::string const& port);
	int							addCgiChild(uint32_t events, Client& client);
	std::map<int, CgiChild>&		getCgiChilds(void);
	void						deleteClient(int fd);
	void						deleteCgiChild(int pipe_fd);
	SessionManager				&getSession();
	const SessionManager				&getSession() const;
private:
	std::vector<int>			_sfds;
	time_t						_last_check;
	time_t						_last_check_scripts;
	int							_epollfd;
	std::vector<serverConfig> const&	_server_conf;
	struct epoll_event			_einf, _eventQueue[MAX_EVENTS];
	std::map<int, Client>		_clients;
	std::map<int, CgiChild>		_scripts_childs;
	SessionManager				_sessions;
};
