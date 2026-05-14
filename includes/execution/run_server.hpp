/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   run_server.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/29 19:14:10 by acamargo          #+#    #+#             */
/*   Updated: 2026/05/14 20:42:33 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <netdb.h>

#include <string>

#include <sys/poll.h>

#include <sys/socket.h>

#include <netinet/in.h>

#include <sys/types.h>

#include <unistd.h>

#include <poll.h>

#include <vector>

#include <iostream>

#include "../includes/configClass/serverConfig.hpp"

#define TIMEOUT 0

int	run(std::vector<serverConfig> const&	servers_conf);
