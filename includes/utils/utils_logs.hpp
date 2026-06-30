/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_logs.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/29 14:44:40 by acamargo          #+#    #+#             */
/*   Updated: 2026/06/30 21:38:13 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>

#include <utils.hpp>

#include <Request.hpp>

#define TEXT_RED "31"
#define TEXT_WHITE "37"
#define TEXT_YELLOW "33"
#define TEXT_GREEN "32"
#define TEXT_BLUE "34"
#define TEXT_MAGENTA "35"
#define TEXT_CYAN "36"

void	print_log(std::string const text_color,
				Request::ErrorRequest const* e,
				std::string const log, bool is_error);
