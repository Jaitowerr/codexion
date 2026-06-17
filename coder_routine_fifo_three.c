/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_routine_fifo_three.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aitorres <aitorres@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 00:00:00 by aitorres          #+#    #+#             */
/*   Updated: 2026/06/15 18:23:46 by aitorres         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders/codexion.h"

void	remove_from_queue_fifo(t_dongle *dongle, t_request *req)
{
	t_request	**cur;

	cur = &dongle->wait_queue;
	while (*cur && *cur != req)
		cur = &(*cur)->next;
	if (*cur)
		*cur = req->next;
}

void	cleanup_on_burnout_single_fifo(t_dongle *first)
{
	pthread_mutex_lock(&first->mutex);
	first->taken = false;
	pthread_mutex_unlock(&first->mutex);
}

void	get_dongle_order_fifo(t_coder *self, t_dongle **first,
	t_dongle **second)
{
	if (self->id % 2 == 0)
	{
		*first = self->right_dongle;
		*second = self->left_dongle;
	}
	else
	{
		*first = self->left_dongle;
		*second = self->right_dongle;
	}
}
