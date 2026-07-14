/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   megaphone.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/28 16:33:03 by acamargo          #+#    #+#             */
/*   Updated: 2026/02/03 11:28:30 by alejandrocama    ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>

int	main(int argc, char **argv)
{
	int			i;
	std::string	output;

	i = 1;
	if (argc <= 1)
	{
		std::cout << "* LOUD AND UNBEARABLE FEEDBACK NOISE *" << "\n";
		return (1);
	}
	while (i < argc)
	{
		output = argv[i];
		for (unsigned int j = 0; j < output.length(); j++)
			output[j] = std::toupper(output[j]);
		std::cout << output;
		i++;
	}
	std::cout << std::endl;
	return (0);
}
