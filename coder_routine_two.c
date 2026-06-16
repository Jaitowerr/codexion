/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_routine_two.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aitorres <aitorres@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 00:00:00 by jaitowerr         #+#    #+#             */
/*   Updated: 2026/06/10 16:26:50 by aitorres         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders/codexion.h"


static void	release_one_dongle(t_dongle *dongle, long next_available)
{
	pthread_mutex_lock(&dongle->mutex);
	dongle->available_at_ms = next_available;
	if (dongle->wait_queue)
	{
		// Lo dejamos taken = true, ya está reservado para el siguiente
		dongle->wait_queue->granted = true;
		pthread_cond_broadcast(&dongle->wait_queue->cond);
	}
	else
		dongle->taken = false;  // Solo liberamos si nadie espera
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

bool	do_debug(t_coder *self)
{
    if (check_burnout(self)) return (true);
    log_status(self, "is debugging"); // Primero el mensaje
	usleep(self->config->time_to_debug * 1000); // Luego el tiempo
	return (check_burnout(self));
}

bool	do_refactor(t_coder *self)
{
    if (check_burnout(self)) return (true);
    log_status(self, "is refactoring"); // Primero el mensaje
	usleep(self->config->time_to_refactor * 1000); // Luego el tiempo
	return (check_burnout(self));
}
