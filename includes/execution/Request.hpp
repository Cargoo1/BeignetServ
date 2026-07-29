/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/08 14:23:48 by acamargo          #+#    #+#             */
/*   Updated: 2026/07/29 23:39:15 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Header.hpp"
#include "HttpResponse.hpp"
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
		HttpResponse&		getResponse(void);
		//void					setResponse(std::string const& str);
		size_t				getBytesRead(void) const;
		unsigned long long const&	getBodyLen(void) const;
		bool				addBytesRead(size_t bytes);
		void				setBodyLen(unsigned long long const& len);
		bool				addBodyLen(size_t bytes);
		locationConfig const*	getLocConfBlock(void) const;
		void					setLocConfBlock(locationConfig const& loc_conf);
		serverConfig const*	getServerBlock(void) const;
		int					setServerBlock(serverConfig const& server_block);
		std::string&		getBoundary(void);
		void				setBoundary(std::string const& str);
		std::string&	getRawBody(void);
		std::stringstream&	getRequestStream(void);
		bool				is_body_read;
		bool				is_body_being_read;
		bool				waiting_chunk;
		bool				is_request_in_progress;
		size_t				bytes_2_read;
		std::string			data_name;
		bool				setCurrentCgiPipeFd(int pipe_fd);
		int					getCurrentCgiPipeFd(void) const;
		std::string&		getScripOutput(void);

	private:
		Header					_header;
		std::string				_body;
		std::string				_request;
		std::string				_boundary;
		std::stringstream		_raw_body;
		std::stringstream		_request_stream;
		std::string				_script_output;
		int						_current_cgi_pipe_fd;
		HttpResponse			_response;
		unsigned long long		_body_len;
		locationConfig const*	_locConf_block;
		serverConfig const*		_server_block;
		size_t					_bytes_read;
};

