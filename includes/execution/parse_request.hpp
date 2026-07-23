/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_request.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandrocamargo <acamargo@student.42.fr>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/11 19:29:40 by alejandrocama     #+#    #+#             */
/*   Updated: 2026/07/23 20:39:42 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <istream>

#include <string>
#include <utils.hpp>

#include "Request.hpp"

#define REQ_NOT_CMPLETE 1;

typedef void(Header::*field_function)(std::string &);

int	parse_fields(std::string& request,
						Request& r);
