/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_executed.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aitorres <aitorres@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 00:00:00 by aitorres          #+#    #+#             */
/*   Updated: 2026/06/15 18:20:53 by aitorres         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders/codexion.h"

static bool	take_dongles_wrapper(t_coder *self)
{
	if (strcmp(self->config->scheduler, "fifo") == 0)
	{
		if (take_dongles_fifo(self))
			return (true);
	}
	else
	{
		if (take_dongles_edf(self))
			return (true);
	}
	return (false);
}

void	*coder_executed(void *arg)
{
	t_coder	*self;

	self = (t_coder *)arg;
	while (self->compile_count < self->config->number_of_compiles_required
		&& !self->ctx->someone_burned)
	{
		self->last_compile_ms = get_current_time_ms();
		if (take_dongles_wrapper(self))
			return (NULL);
		if (compile(self))
		{
			release_and_cooldown(self);
			return (NULL);
		}
		release_and_cooldown(self);
		if (self->compile_count >= self->config->number_of_compiles_required)
			break ;
		if (do_debug(self))
			return (NULL);
		if (do_refactor(self))
			return (NULL);
	}
	return (NULL);
}
