/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Header.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/19 18:35:09 by acamargo          #+#    #+#             */
/*   Updated: 2026/06/15 12:46:21 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <map>
#include <string>

class Header
{
	public:
		Header();
		Header(Header const& other);
		~Header();

		Header&	operator=(Header const& other);

		std::string&						getMethod(void);
		const std::string&						getMethod(void) const;

		std::string&						getTargetResource(void);
		const std::string&						getTargetResource(void) const;

		std::string&						getProtocolV(void);
		const std::string&						getProtocolV(void) const;

		std::map<std::string, std::string>&	getFields(void);
		const std::map<std::string, std::string>&	getFields(void) const;

		std::string&							getContentLenght(void);
		const std::string&							getContentLenght(void) const;

		std::string&						getQueryStr(void);

		bool	setMethod(std::string& method);
		void	setTargetResource(std::string& uri);
		bool	setProtocolV(std::string& protocol);
		void	setHost(std::string& host);
		void	setContent_len(std::string& content_len);
		bool	is_header_parsed(void) const;
		void	set_is_header_parsed(bool value);
	private:
		std::map<std::string, std::string>	_map_fields;
		bool								_is_header_parsed;
		bool								_is_cgi;
		std::string							_method;
		std::string							_target_resource; //path
		std::string							_query_string;
		std::string							_protocol_v;
};
