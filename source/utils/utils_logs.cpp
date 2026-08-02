/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_logs.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/29 14:15:09 by acamargo          #+#    #+#             */
/*   Updated: 2026/08/02 22:41:13 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <utils_logs.hpp>

void	print_log(std::string const& text_color, Request::ErrorRequest const* e, std::string const& log, bool is_error)
{
	std::string	str_temp;
	if (!e)
	{
		str_temp = "\033[" + text_color + "m" + log + "\033[m\n";
		if (!is_error)
			std::cout << str_temp;
		else
			std::cerr << str_temp;
		return;
	}
	str_temp = "\033[" + text_color + "m" + "[ERROR " + generate_reason_phrase(e->getErrorCode()) + ": " + log + "]" + "\033[m\n"; 
	std::cerr << str_temp;
}
