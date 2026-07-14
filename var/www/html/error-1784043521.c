/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/14 15:58:08 by acamargo          #+#    #+#             */
/*   Updated: 2025/07/14 15:59:49 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/get_next_line.h"

void	ft_lstdelete(t_gnl **lst)
{
	t_gnl	*temp;
	t_gnl	*next;

	if (!lst)
		return ;
	temp = *lst;
	while (temp != NULL)
	{
		next = temp->next;
		free(temp->line);
		free(temp);
		temp = next;
	}
	*lst = NULL;
}
