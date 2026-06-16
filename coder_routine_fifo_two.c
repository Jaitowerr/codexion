/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_routine_fifo_two.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aitorres <aitorres@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 00:00:00 by aitorres          #+#    #+#             */
/*   Updated: 2026/06/15 18:23:46 by aitorres         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders/codexion.h"

void	init_request_fifo(t_request *req, t_coder *self)
{
	req->coder_id = self->id;
	req->granted  = false;
	req->next     = NULL;
}

void	insert_in_queue_fifo(t_dongle *dongle, t_request *req)
{
	t_request	**cur;

	cur = &dongle->wait_queue;
	while (*cur)
		cur = &(*cur)->next;
	*cur = req;
}


void	wait_for_grant_or_burnout_fifo(t_dongle *dongle, t_request *req,
	t_coder *self)
{
	while (!req->granted && !check_burnout(self))
		pthread_cond_wait(&req->cond, &dongle->mutex);
}

void	handle_burnout_fifo(t_dongle *dongle, t_request *req)
{
	if (req->granted)
	{
		dongle->taken = false;
		if (dongle->wait_queue)
		{
			dongle->wait_queue->granted = true;
			dongle->taken = true;
			pthread_cond_broadcast(&dongle->wait_queue->cond);
		}
	}
}
void	cleanup_on_burnout_both_fifo(t_dongle *first, t_dongle *second)
{
	pthread_mutex_lock(&first->mutex);
	first->taken = false;
	pthread_mutex_unlock(&first->mutex);
	pthread_mutex_lock(&second->mutex);
	second->taken = false;
	pthread_mutex_unlock(&second->mutex);
}
