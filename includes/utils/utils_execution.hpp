/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_execution.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/22 20:17:01 by acamargo          #+#    #+#             */
/*   Updated: 2026/08/03 16:29:28 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <run_server.hpp>

#include <Server.hpp>

int		writeall(int fd, char const* buf, size_t &len);

int		sendall(int fd, char const* buf, size_t &len);

void	close_pipe(int* pipe);

int		handle_script_output(CgiChild& child);

void	generateTimestampFilename(std::string& filename);

int		find_filename(Header const& header, std::string& filename);
