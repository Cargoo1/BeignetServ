/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_request.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandrocamargo <acamargo@student.42.fr>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/11 19:40:38 by alejandrocama     #+#    #+#             */
/*   Updated: 2026/05/14 17:06:21 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <exception>
#include <iostream>
#include <sstream>
#include <string>
#include "../includes/parse_request.hpp"

int	handle_request(std::string& request)
{
	Request	r;
	int		status_code = 200;
	(void)status_code;
	size_t pos = request.find("\r\n\r\n");
	std::istringstream request_stream(request.substr(0, pos));
	std::string test = request_stream.str();

	try
	{
		parse_request(request_stream, r);
		///////
	}
	catch(std::exception& e)
	{
		std::cerr << e.what();
		status_code = 400;
	}
	std::cout << *r.getMethod() << std::endl;
	std::cout << *r.getProtocolV() << std::endl;
	std::cout << *r.getTargetResource() << std::endl;
	std::cout << r.getHost().at("Host") << '\n';
	return 200;	
}
