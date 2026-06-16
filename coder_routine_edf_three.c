/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_routine_edf_three.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aitorres <aitorres@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 00:00:00 by aitorres          #+#    #+#             */
/*   Updated: 2026/06/15 18:23:46 by aitorres         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders/codexion.h"


void	handle_burnout_edf(t_dongle *dongle, t_request *req)
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

void	cleanup_on_burnout_both_edf(t_dongle *first, t_dongle *second)
{
	pthread_mutex_lock(&first->mutex);
	first->taken = false;
	pthread_mutex_unlock(&first->mutex);
	pthread_mutex_lock(&second->mutex);
	second->taken = false;
	pthread_mutex_unlock(&second->mutex);
}
void	cleanup_on_burnout_single_edf(t_dongle *first)
{
	pthread_mutex_lock(&first->mutex);
	first->taken = false;
	pthread_mutex_unlock(&first->mutex);
}

void	get_dongle_order_edf(t_coder *self, t_dongle **first, t_dongle **second)
{
	if (self->id % 2 == 0)
	{
		*first  = self->right_dongle;
		*second = self->left_dongle;
	}
	else
	{
		*first  = self->left_dongle;
		*second = self->right_dongle;
	}
}
void	handle_immediate_dongle_acquisition(t_dongle *dongle, t_request *req,
				t_coder *self)
{
	if (!dongle->taken && get_current_time_ms() >= dongle->available_at_ms)
	{
		dongle->taken = true;
		pthread_mutex_unlock(&dongle->mutex);
		pthread_cond_destroy(&req->cond);
		return ;
	}
	insert_in_queue_edf(dongle, req);
	wait_for_grant_or_burnout_edf(dongle, req, self);
	remove_from_queue_edf(dongle, req);
}
