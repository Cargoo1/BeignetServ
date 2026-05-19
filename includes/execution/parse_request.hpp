/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_request.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandrocamargo <acamargo@student.42.fr>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/11 19:29:40 by alejandrocama     #+#    #+#             */
/*   Updated: 2026/05/19 20:32:44 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <istream>
#include "Request.hpp"

void	parse_header(std::istringstream& request, Header &header);
