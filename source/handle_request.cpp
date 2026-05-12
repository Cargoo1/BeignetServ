/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_request.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandrocamargo <acamargo@student.42.fr>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/11 19:40:38 by alejandrocama     #+#    #+#             */
/*   Updated: 2026/05/12 23:13:36 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <exception>
#include <iostream>
#include <sstream>
#include <string>
#include "../includes/parse_request.hpp"

void	handle_request(std::string& request)
{
	Request	r;
	std::istringstream request_stream(request);

	try
	{
		parse_request(request_stream, r);
	}
	catch(std::exception& e)
	{
		std::cerr << e.what();
		return;
	}
	std::cout << *r.getMethod() << std::endl;
	std::cout << *r.getProtocolV() << std::endl;
	std::cout << *r.getTargetResource() << std::endl;
	std::cout << r.getHost().at("host") << '\n';
}

