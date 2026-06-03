/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_request.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandrocamargo <acamargo@student.42.fr>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/11 19:29:40 by alejandrocama     #+#    #+#             */
/*   Updated: 2026/06/03 17:53:11 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <istream>

#include <utils.hpp>

#include "Request.hpp"

void	parse_header(std::istringstream& request, Header &header);
