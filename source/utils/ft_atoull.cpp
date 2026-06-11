/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoull.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 17:45:10 by acamargo          #+#    #+#             */
/*   Updated: 2026/06/11 18:15:41 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <climits>
#include <cstddef>

static bool	ft_str_is_numeric(char c)
{
	return (c >= '0' && c <= '9');
}

static bool	ft_isspace(char c)
{
	return ((c >= '\t' && c <= '\r') || c == ' ');
}

static unsigned long long	convertion(char const*str)
{
	size_t	i;
	unsigned long long	num;

	i = 0;
	num = 0;
	while (ft_str_is_numeric(str[i]))
	{
		if (num > ULONG_LONG_MAX / 10 || (num == ULONG_LONG_MAX / 10 && str[i] - '0' > 5))
				return (ULONG_LONG_MAX);
		num = num * 10 + (str[i] - '0');
		i++;
	}
	return (num);
}

unsigned long long	ft_atoull(char const* str)
{
	int		i;
	unsigned long long	num;

	if (!str)
		return (0);
	i = 0;
	while (ft_isspace(str[i]))
		i++;
	num = convertion(&str[i]);
	return (num);
}
