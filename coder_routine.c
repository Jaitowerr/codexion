/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_routine.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aitorres <aitorres@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 00:00:00 by aitorres          #+#    #+#             */
/*   Updated: 2026/06/15 18:23:46 by aitorres         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders/codexion.h"

void	wait_cooldown_fifo_edf(t_dongle *dongle, t_request *req, t_coder *self)
{
	struct timespec	ts;
	long long		now;

	while (!check_burnout(self))
	{
		now = get_current_time_ms();
		if (now >= dongle->available_at_ms)
			break ;
		ts.tv_sec = (now + 50) / 1000;
		ts.tv_nsec = ((now + 50) % 1000) * 1000000;
		pthread_cond_timedwait(&req->cond, &dongle->mutex, &ts);
	}
}

bool	check_burnout(t_coder *self)
{
	pthread_mutex_lock(&self->ctx->burnout_mutex);
	if (self->ctx->someone_burned)
	{
		pthread_mutex_unlock(&self->ctx->burnout_mutex);
		return (true);
	}
	pthread_mutex_unlock(&self->ctx->burnout_mutex);
	return (false);
}

bool	compile(t_coder *self)
{
	if (check_burnout(self))
		return (true);
	log_status(self, "is compiling");
	self->last_compile_ms = get_current_time_ms();
	return (sleep_with_burnout_check(self, self->config->time_to_compile));
}
