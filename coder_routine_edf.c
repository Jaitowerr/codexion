/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_routine_edf.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aitorres <aitorres@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 00:00:00 by aitorres          #+#    #+#             */
/*   Updated: 2026/06/15 18:23:46 by aitorres         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders/codexion.h"

static void	handle_dongle_post_acquisition(t_dongle *dongle, t_request *req,
				t_coder *self)
{
	if (check_burnout(self))
	{
		handle_burnout_edf(dongle, req);
		pthread_mutex_unlock(&dongle->mutex);
		pthread_cond_destroy(&req->cond);
		return ;
	}
	wait_cooldown_fifo_edf(dongle, req, self);
	pthread_mutex_unlock(&dongle->mutex);
	pthread_cond_destroy(&req->cond);
}

static void	take_one_dongle_edf(t_dongle *dongle, t_coder *self)
{
	t_request	req;

	init_request_edf(&req, self);
	pthread_cond_init(&req.cond, NULL);
	pthread_mutex_lock(&dongle->mutex);
	handle_immediate_dongle_acquisition(dongle, &req, self);
	handle_dongle_post_acquisition(dongle, &req, self);
}

static bool	handle_single_or_burnout_case(t_coder *self)
{
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
	return (false);
}

static bool	acquire_both_dongles(t_coder *self)
{
	t_dongle	*first;
	t_dongle	*second;

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

bool	take_dongles_edf(t_coder *self)
{
	if (handle_single_or_burnout_case(self))
		return (true);
	return (acquire_both_dongles(self));
}
