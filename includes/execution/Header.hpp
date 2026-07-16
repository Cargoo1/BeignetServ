/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Header.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/19 18:35:09 by acamargo          #+#    #+#             */
/*   Updated: 2026/07/16 21:41:17 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <map>

#include <string>

class	Request;

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

		std::string const&						getQueryStr(void) const;
		std::string const&						getPathInfo(void) const;

		bool	setMethod(std::string& method);
		void	setTargetResource(std::string& uri, Request& r);
		bool	setProtocolV(std::string& protocol);
		void	setHost(std::string& host);
		void	setContent_len(std::string& content_len);
		void	setContent_type(std::string& content_type);
		std::string const*	getContentType(void) const;
		void	setContent_dispo(std::string& content_type);
		std::string&	getBoundary(void);
		void	setTransfer_encoding(std::string& transfer_encoding);
		bool	is_header_parsed(void) const;
		void	set_is_header_parsed(bool value);
		bool	is_a_script(void) const;
		std::string const&	getFilename(void) const;
		void				setFilename(std::string const& str);
		std::string			data_values[2];
		bool				is_method_parsed;
	private:
		std::map<std::string, std::string>	_map_fields;
		bool								_is_header_parsed;
		bool								_is_a_script;
		std::string							_method;
		std::string							_boundary;
		std::string							_target_resource; //path
		std::string							_query_string;
		std::string							_path_info;
		std::string							_protocol_v;
		std::string							_file_name;
};
