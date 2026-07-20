/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/14 19:47:36 by acamargo          #+#    #+#             */
/*   Updated: 2026/07/20 16:16:28 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Request.hpp"
#include <ctime>
#include <sys/epoll.h>
class Client
{
friend	class Server;
public:
	Client(int fd, uint32_t events);
	Client(Client const& other);
	~Client();

	Client&	operator=(Client const& other);

	std::string const&	getIp(void) const;
	std::string const&	getPort(void) const;
	time_t			getLastComm(void) const;
	void			setLastComm(void);
	Request&		getRequest(void);
	void			setIpPort(std::string const& ip, std::string const& port);
	std::string&	getNotConstMsg(void);
	std::string const&	getMsg(void) const;
	int				getFd(void) const;
private:
	Request		_r;
	time_t		_last_communication;
	std::string	_ip;
	std::string	_port;
	std::string	_message;
	struct epoll_event	_einf;
};
