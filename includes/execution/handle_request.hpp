/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_request.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandrocamargo <acamargo@student.42.fr>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/11 19:51:58 by alejandrocama     #+#    #+#             */
/*   Updated: 2026/06/23 17:32:24 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Client.hpp"
#include <string>
#include <vector>
#include <serverConfig.hpp>

int	handle_request(Client& client);
