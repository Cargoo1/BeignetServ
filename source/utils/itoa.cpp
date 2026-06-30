/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   itoa.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/30 15:18:55 by acamargo          #+#    #+#             */
/*   Updated: 2026/06/30 15:49:41 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>

void	ft_itoa(int n, std::string& str)
{
	if (n < 10)
	{
		str.push_back(n + 48);
		return;
	}
	ft_itoa(n / 10, str);
	str.push_back((n % 10) + 48);
	return ;
}
