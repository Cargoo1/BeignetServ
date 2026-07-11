/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/08 14:23:48 by acamargo          #+#    #+#             */
/*   Updated: 2026/07/11 15:01:41 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Header.hpp"
#include "locationConfig.hpp"
#include <serverConfig.hpp>
#include <cstddef>
#include <exception>
#include <map>
#include <sstream>
#include <string>

#define CRLF "\r\n"
#define DOUBLE_CRLF "\r\n\r\n"

class	Request
{
public:
	class	ErrorRequest : public std::exception
	{
	public:
		ErrorRequest(int error_code, char const* reason);
		int		getErrorCode(void) const;
		virtual const char *	what() const throw();
	private:
		int	_error_code;
		const char* reason;
	};
	Request(void);
	Request(const Request & other);
	~Request();
	
	Request & operator=(const Request& other);

	const Header&		getHeader(void) const;
	Header&				getHeader(void);
	std::string const&	getBody(void) const;
	void				appendBody(std::string const& str);
	std::string const&		getResponse(void) const;
	void					setResponse(std::string const& str);
	bool					getReqInProg(void) const;
	void					setReqInProg(bool value);
	size_t				getBytesRead(void) const;
	unsigned long long const&	getBodyLen(void) const;
	bool				addBytesRead(size_t bytes);
	void				setBodyLen(unsigned long long const& len);
	bool				addBodyLen(size_t bytes);
	locationConfig const*	getLocConfBlock(void) const;
	void					setLocConfBlock(locationConfig const& loc_conf);
	serverConfig const*	getServerBlock(void) const;
	int					setServerBlock(serverConfig const& server_block);
	bool				is_body_read(void) const;
	void				set_is_body_read(bool value);
	std::stringstream&	getRawBody(void);
	std::stringstream&	getRequestStream(void);
	bool				waiting_chunk(void) const;
	void				setWaitingChunk(bool value);

private:
	Header		_header;
	bool		_request_in_progress;
	bool		_is_body_read;
	bool		_waiting_chunk;
	std::string	_body;
	std::stringstream	_raw_body;
	std::stringstream	_request_stream;
	unsigned long long	_body_len;
	locationConfig const*	_locConf_block;
	serverConfig const*	_server_block;
	size_t		_bytes_read;
	std::string	_response;
};
