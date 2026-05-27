/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/08 14:23:48 by acamargo          #+#    #+#             */
/*   Updated: 2026/05/27 15:07:20 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Header.hpp"
#include <exception>
#include <map>
#include <string>

class	Request
{
public:
	class	ErrorRequest : public std::exception
	{
	public:
		ErrorRequest(int error_code);
		int		getErrorCode(void) const;
		virtual const char *	what() const throw();
	private:
		int	_error_code;
	};
	Request(void);
	Request(const Request & other);
	~Request();
	
	Request & operator=(const Request& other);

	const Header&		getHeader(void) const;
	Header&				getHeader(void);
	const std::string	getBody(void) const;
	std::string	getBody(void);
private:
	Header		_header;
	std::string	_body;
};
