/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   send_http_response.hpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 23:09:27 by acamargo          #+#    #+#             */
/*   Updated: 2026/06/09 14:48:22 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Request.hpp"
#include <Client.hpp>
#include <serverConfig.hpp>

int	send_response(Request& r, int status_code, serverConfig const& serverConf,
					int cfd);
