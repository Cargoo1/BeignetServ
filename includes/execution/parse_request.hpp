/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_request.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandrocamargo <acamargo@student.42.fr>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/11 19:29:40 by alejandrocama     #+#    #+#             */
/*   Updated: 2026/07/09 21:31:00 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <istream>

#include <string>
#include <utils.hpp>

#include "Request.hpp"

#define REQ_NOT_CMPLETE 1;

int	parse_header(std::string& request,
						Request& r);
