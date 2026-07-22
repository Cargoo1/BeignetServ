/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/18 19:30:27 by acamargo          #+#    #+#             */
/*   Updated: 2026/07/22 23:04:44 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <unistd.h>
int	main(int argc, char** argv, char** env)
{
	(void)argc;
	(void)argv;
	for (size_t i = 0; env[i]; i++)
	{
		printf("%s\n", env[i]);
		sleep(2);
	}
	printf("END\n");
	return 1;
}
