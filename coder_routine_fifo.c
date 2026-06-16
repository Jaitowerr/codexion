/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_routine_fifo.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aitorres <aitorres@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 00:00:00 by aitorres          #+#    #+#             */
/*   Updated: 2026/06/15 18:23:46 by aitorres         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders/codexion.h"


static void	queue_and_wait_fifo(t_dongle *dongle, t_request *req,
				t_coder *self)
{
	insert_in_queue_fifo(dongle, req);
	wait_for_grant_or_burnout_fifo(dongle, req, self);
	remove_from_queue_fifo(dongle, req);
}


static void	take_one_dongle_fifo(t_dongle *dongle, t_coder *self)
{
	t_request	req;

	init_request_fifo(&req, self);
	pthread_cond_init(&req.cond, NULL);
	pthread_mutex_lock(&dongle->mutex);
	if (!dongle->taken && get_current_time_ms() >= dongle->available_at_ms)
	{
		dongle->taken = true;
		pthread_mutex_unlock(&dongle->mutex);
		pthread_cond_destroy(&req.cond);
		return ;
	}
	queue_and_wait_fifo(dongle, &req, self);
	if (check_burnout(self))
	{
		handle_burnout_fifo(dongle, &req);
		pthread_mutex_unlock(&dongle->mutex);
		pthread_cond_destroy(&req.cond);
		return ;
	}
	wait_cooldown_fifo_edf(dongle, &req, self);
	pthread_mutex_unlock(&dongle->mutex);
	pthread_cond_destroy(&req.cond);
}

static bool	handle_single_or_burnout_fifo(t_coder *self)
{
	if (check_burnout(self))
		return (true);
	if (self->left_dongle == self->right_dongle)
	{
		take_one_dongle_fifo(self->left_dongle, self);
		log_status(self, "has taken a dongle");
		while (!check_burnout(self))
			usleep(1000);
		return (true);
	}
	return (false);
}


static bool	acquire_two_dongles_fifo(t_coder *self)
{
	t_dongle	*first;
	t_dongle	*second;

	get_dongle_order_fifo(self, &first, &second);
	take_one_dongle_fifo(first, self);
	if (check_burnout(self))
	{
		cleanup_on_burnout_single_fifo(first);
		return (true);
	}
	log_status(self, "has taken a dongle");
	take_one_dongle_fifo(second, self);
	if (check_burnout(self))
	{
		cleanup_on_burnout_both_fifo(first, second);
		return (true);
	}
	log_status(self, "has taken a dongle");
	return (false);
}


bool	take_dongles_fifo(t_coder *self)
{
	if (handle_single_or_burnout_fifo(self))
		return (true);
	return (acquire_two_dongles_fifo(self));
}
