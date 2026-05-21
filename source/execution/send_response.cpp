/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   send_response.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/21 21:54:03 by acamargo          #+#    #+#             */
/*   Updated: 2026/05/21 22:36:52 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <HttpResponse.hpp>
#include <vector>
#include <serverConfig.hpp>

void	client_error(HttpResponse& response)
{

}

int	send_response(int status_code, serverConfig const& serverConf)
{
	HttpResponse	response(status_code);
	response.addField("Server", "Beignetserv/0.1");
	if (status_code >= 400)
	{

	}
}
