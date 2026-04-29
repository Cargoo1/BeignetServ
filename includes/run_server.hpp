/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   run_server.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/29 19:14:10 by acamargo          #+#    #+#             */
/*   Updated: 2026/04/29 20:16:53 by acamargo         ###   ########.fr       */
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

#include "Config.hpp"

#include <iostream>

void	run(std::vector<int> ports, std::string &interface);
