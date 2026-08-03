/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   run_server.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/29 19:14:10 by acamargo          #+#    #+#             */
/*   Updated: 2026/08/03 20:03:16 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <netdb.h>

#include <sys/poll.h>

#include <sys/socket.h>

#include <netinet/in.h>

#include <errno.h>

#include <sys/types.h>

#include <unistd.h>

#include <poll.h>

#include <vector>

#include <serverConfig.hpp>

#include <utils.hpp>

#define TIMEOUT 0

#define RUN_CGI	2

#define INCOMPLETE 1

#define DONE 0

extern volatile bool stop_server;

int	run(std::vector<serverConfig> const&	servers_conf);

void	get_client_request(Server& server, int fd);
