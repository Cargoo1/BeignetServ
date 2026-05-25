/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Header.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/19 18:35:09 by acamargo          #+#    #+#             */
/*   Updated: 2026/05/19 20:26:10 by acamargo         ###   ########.fr       */
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
		std::string&						getTargetResource(void);
		const std::string&					getTargetResource(void) const;
		std::string&						getProtocolV(void);
		std::map<std::string, std::string>&	getFields(void);
		std::string							getContentLenght(void);
		const std::string							getContentLenght(void) const;
		bool	setMethod(std::string& method);
		bool	setTargetResource(std::string& uri);
		bool	setProtocolV(std::string& protocol);
		void	setHost(std::string& host);
		void	setContent_len(std::string& content_len);
	private:
		std::map<std::string, std::string>	_map_fields;
		std::string							_method;
		std::string							_target_resource; //path
		std::string							_protocol_v;
};
