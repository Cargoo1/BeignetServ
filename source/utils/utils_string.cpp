/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_string.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/01 13:06:56 by acamargo          #+#    #+#             */
/*   Updated: 2026/07/23 19:50:08 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include <cstring>
#include <string>
#include <utils.hpp>

bool	remove_cr(std::string& str)
{
	if (str.empty())
		return false;
	size_t	cr_pos = str.find_last_of('\r');
	if (cr_pos == str.npos || cr_pos != str.length() - 1)
		return false;
	str.erase(cr_pos);
	if (str.empty())
		return false;
	return true;
}

void	consume_until_crlf(std::string& str)
{
	size_t	crlf_pos = str.find(CRLF);
	if (crlf_pos == str.npos)
		return;
	str.erase(0, crlf_pos + std::strlen(CRLF));
}
