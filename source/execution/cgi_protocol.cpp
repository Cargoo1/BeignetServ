/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi_protocol.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/30 19:17:45 by acamargo          #+#    #+#             */
/*   Updated: 2026/06/30 20:54:03 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponse.hpp"
#include "Request.hpp"
#include "locationConfig.hpp"
/*
	
*/
void	execute_script(Request const& r, HttpResponse& response)
{
	locationConfig const&	loc_conf = *r.getLocConfBlock();
	
}
