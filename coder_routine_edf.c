/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_routine.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aitorres <aitorres@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 00:00:00 by jaitowerr         #+#    #+#             */
/*   Updated: 2026/06/15 18:23:46 by aitorres         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders/codexion.h"





static void	cleanup_on_burnout_single_edf(t_dongle *first)
{
	pthread_mutex_lock(&first->mutex);
	first->taken = false;
	pthread_mutex_unlock(&first->mutex);
}

static void	get_dongle_order_edf(t_coder *self, t_dongle **first, t_dongle **second)
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

static void	take_one_dongle_edf(t_dongle *dongle, t_coder *self)
{
	t_request	req;

	init_request_edf(&req, self);
	pthread_cond_init(&req.cond, NULL);
	pthread_mutex_lock(&dongle->mutex);

	if (!dongle->taken && get_current_time_ms() >= dongle->available_at_ms)
	{
		dongle->taken = true;
		pthread_mutex_unlock(&dongle->mutex);
		pthread_cond_destroy(&req.cond);
		return ;
	}

	insert_in_queue_edf(dongle, &req);
	wait_for_grant_or_burnout_edf(dongle, &req, self);
	remove_from_queue_edf(dongle, &req);

	if (check_burnout(self))
	{
		handle_burnout_edf(dongle, &req);
		pthread_mutex_unlock(&dongle->mutex);
		pthread_cond_destroy(&req.cond);
		return ;
	}

	wait_cooldown_fifo_edf(dongle, &req, self);
	pthread_mutex_unlock(&dongle->mutex);
	pthread_cond_destroy(&req.cond);
}

bool	take_dongles_edf(t_coder *self)
{
	t_dongle	*first;
	t_dongle	*second;

	if (check_burnout(self))
		return (true);

	if (self->left_dongle == self->right_dongle)
	{
		take_one_dongle_edf(self->left_dongle, self);
		log_status(self, "has taken a dongle");
		while (!check_burnout(self))
			usleep(1000);
		return (true);
	}

	get_dongle_order_edf(self, &first, &second);
	take_one_dongle_edf(first, self);

	if (check_burnout(self))
	{
		cleanup_on_burnout_single_edf(first);
		return (true);
	}
	log_status(self, "has taken a dongle");

	take_one_dongle_edf(second, self);
	if (check_burnout(self))
	{
		cleanup_on_burnout_both_edf(first, second);
		return (true);
	}
	log_status(self, "has taken a dongle");

	return (false);
}
