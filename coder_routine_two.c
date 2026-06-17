/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_routine_two.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aitorres <aitorres@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 00:00:00 by aitorres          #+#    #+#             */
/*   Updated: 2026/06/17 16:24:34 by aitorres         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders/codexion.h"

static void	release_one_dongle(t_dongle *dongle, long next_available)
{
	pthread_mutex_lock(&dongle->mutex);
	dongle->available_at_ms = next_available;
	if (dongle->wait_queue)
	{
		dongle->wait_queue->granted = true;
		pthread_cond_broadcast(&dongle->wait_queue->cond);
	}
	else
		dongle->taken = false;
	pthread_mutex_unlock(&dongle->mutex);
}

bool	release_and_cooldown(t_coder *self)
{
	long	next_available;

	next_available = get_current_time_ms() + self->config->dongle_cooldown;
	release_one_dongle(self->left_dongle, next_available);
	if (self->left_dongle != self->right_dongle)
		release_one_dongle(self->right_dongle, next_available);
	self->compile_count++;
	return (false);
}

bool	sleep_with_burnout_check(t_coder *self, long ms)
{
	long	slept;
	long	chunk;

	slept = 0;
	while (slept < ms)
	{
		if (check_burnout(self))
			return (true);
		chunk = ms - slept;
		if (chunk > 50)
			chunk = 50;
		usleep(chunk * 1000);
		slept += chunk;
	}
	return (check_burnout(self));
}

bool	do_debug(t_coder *self)
{
	if (check_burnout(self))
		return (true);
	log_status(self, "is debugging");
	return (sleep_with_burnout_check(self, self->config->time_to_debug));
}

bool	do_refactor(t_coder *self)
{
	if (check_burnout(self))
		return (true);
	log_status(self, "is refactoring");
	return (sleep_with_burnout_check(self, self->config->time_to_refactor));
}
