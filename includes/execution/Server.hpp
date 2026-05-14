/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 15:08:39 by acamargo          #+#    #+#             */
/*   Updated: 2026/05/14 20:11:51 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Request.hpp"
#include "configClass/serverConfig.hpp"
#include <sys/poll.h>
#include <vector>
#include <Client.hpp>

class	Server
{
public:
	Server(Server const& other);
	Server(serverConfig const& server_conf, int sfd, short events);
	~Server();

	Server& operator=(Server const& other);

	std::vector<struct pollfd>&	getPfd(void);
	int							getSfd(void);
	std::vector<Client>&		getClients(void);
	void						setRequest(Request const& r);
	void						setMessage(std::string const& r);
	void						addPfd(int pfd, short events);
	bool						addClient(int fd);
private:
	std::vector<Client>			_clients;
	serverConfig const&			_server_conf;
	int							_sfd;
	std::vector<struct pollfd>	_connections;
};
