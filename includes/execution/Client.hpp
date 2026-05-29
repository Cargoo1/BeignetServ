/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/14 19:47:36 by acamargo          #+#    #+#             */
/*   Updated: 2026/05/29 17:01:21 by acamargo         ###   ########.fr       */
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

	std::string const&	getMessage(void) const;
	std::string const&	getResponse(void) const;
	std::string const&	getIp(void) const;
	std::string const&	getPort(void) const;
	Request&		getRequest(void);
	void			setMessage(std::string const& msg);
	void			appendMessage(std::string const& s);
	void			setResponse(std::string& response);
	void			setIpPort(std::string const& ip, std::string const& port);
	int				getFd(void) const;
private:
	Request		_r;
	time_t		_last_comunication;
	std::string	_ip;
	std::string	_port;
	std::string	_message;
	std::string	_response;
	struct epoll_event	_einf;
};
