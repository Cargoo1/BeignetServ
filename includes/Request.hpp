/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/08 14:23:48 by acamargo          #+#    #+#             */
/*   Updated: 2026/05/13 18:59:26 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <exception>
#include <map>
#include <string>

class	Request
{
public:
	class	BadRequest : public std::exception
	{
	public:
		virtual const char *	what() const throw();
	};
	Request(std::string &method, std::string& uri, std::string& protocol_v,
			std::string& host, std::string& port);
	Request(void);
	Request(const Request & other);
	~Request();

	std::string const*	getMethod(void) const;
	std::string const*	getTargetResource(void) const;
	std::string const*	getProtocolV(void) const;
	std::map<std::string const, std::string> const & getHost(void) const;
	bool	setMethod(std::string& method);
	bool	setTargetResource(std::string& uri);
	bool	setProtocolV(std::string& protocol);
	bool	setHost(std::string& host);

	Request & operator=(const Request& other);
private:
	int			status_code;
	std::string	*_method;
	std::string	*_target_resource;
	std::string	*_protocol_v;
	std::map<std::string const, std::string>	_host;
};
