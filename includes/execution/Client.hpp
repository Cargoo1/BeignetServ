/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/14 19:47:36 by acamargo          #+#    #+#             */
/*   Updated: 2026/05/14 20:35:10 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Request.hpp"
class Client
{
friend	class Server;
public:
	Client(int fd);
	Client(Client const& other);
	~Client();

	Client&	operator=(Client const& other);

	std::string&	getMessage(void);
	std::string&	getResponse(void);
	void			setMessage(std::string& msg);
	void			setResponse(std::string& response);
private:
	Request		_r;
	std::string	message;
	std::string	response;
	int			fd;
};
