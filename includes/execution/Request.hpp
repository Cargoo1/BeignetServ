/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/08 14:23:48 by acamargo          #+#    #+#             */
/*   Updated: 2026/05/19 19:35:35 by acamargo         ###   ########.fr       */
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
	class	BadRequest : public std::exception
	{
	public:
		virtual const char *	what() const throw();
	};
	Request(void);
	Request(const Request & other);
	~Request();

	const Header&				getHeader(void) const;
	Header&				getHeader(void);

	Request & operator=(const Request& other);
private:
	int			status_code;
	Header		_header;
	std::string	_body;
};
