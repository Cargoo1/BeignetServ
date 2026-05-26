/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/14 19:47:36 by acamargo          #+#    #+#             */
/*   Updated: 2026/05/26 22:49:57 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Request.hpp"
#include <sys/epoll.h>
class Client
{
friend	class Server;
public:
	Client(int fd, uint32_t events);
	Client(Client const& other);
	~Client();

	Client&	operator=(Client const& other);

	std::string&	getMessage(void);
	std::string&	getResponse(void);
	std::string const&	getIp(void) const;
	std::string const&	getPort(void) const;
	void			setMessage(std::string& msg);
	void			setResponse(std::string& response);
	void			setIpPort(std::string const& ip, std::string const& port);
	int				getFd(void) const;
	Request&		getRequest(void);
private:
	Request		_r;
	std::string	_ip;
	std::string	_port;
	std::string	message;
	std::string	response;
	struct epoll_event	einf;
};
