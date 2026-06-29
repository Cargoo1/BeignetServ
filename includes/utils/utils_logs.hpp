/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_logs.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/29 14:44:40 by acamargo          #+#    #+#             */
/*   Updated: 2026/06/29 14:49:48 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>

#include <utils.hpp>

#include <Request.hpp>

#define TEXT_RED "31"
#define TEXT_WHITE "37"
#define TEXT_YELLOW "33"
#define TEXT_GREE "32"
#define TEXT_BLUE "34"

void	print_log(std::string const text_color,
				Request::ErrorRequest const* e,
				std::string const log, bool is_error);
