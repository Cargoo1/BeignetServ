/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 15:08:39 by acamargo          #+#    #+#             */
/*   Updated: 2026/05/12 15:30:20 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Request.hpp"
#include <sys/poll.h>

class	Client
{
public:
	Client();
	Client(Client const& other);
	Client(int sfd, short events);
	~Client();

	Client& operator=(Client const& other);

	struct pollfd&	getPfd(void);
	std::string&	getMessage(void);
	Request&		getRequest(void);
	void			setRequest(Request const& r);
	void			setMessage(std::string const& r);
	void			setPfd(struct pollfd const& pfd);
private:
	Request	_r;
	std::string	_message;
	struct pollfd	_pfd;
};
