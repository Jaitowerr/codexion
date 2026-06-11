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


bool release_and_cooldown(t_coder *self)
{
    long long   now_ms;
    long long   next_available;

    now_ms = get_current_time_ms();
    next_available = now_ms + self->config->dongle_cooldown;

    pthread_mutex_lock(&self->left_dongle->mutex);
    self->left_dongle->available_at_ms = next_available;
    self->left_dongle->taken = false;
    pthread_cond_broadcast(&self->left_dongle->cond);
    pthread_mutex_unlock(&self->left_dongle->mutex);

    pthread_mutex_lock(&self->right_dongle->mutex);
    self->right_dongle->available_at_ms = next_available;
    self->right_dongle->taken = false;
    pthread_cond_broadcast(&self->right_dongle->cond);
    pthread_mutex_unlock(&self->right_dongle->mutex);

    self->compile_count++;
    return (false);
}


// bool	release_and_cooldown(t_coder *self)
// {
// 	long	now_ms;
// 	long	next_available;

// 	now_ms = get_current_time_ms();
// 	next_available = now_ms + self->config->dongle_cooldown;

// 	//COOLDOWN - TERMINAR Y SOLTAR DONGLES
// 	self->left_dongle->available_at_ms = next_available; //el cooldown le marcamos cuando estará disponible de nuevo
// 	self->left_dongle->taken = false;
// 	pthread_mutex_unlock(&self->left_dongle->mutex);

// 	self->right_dongle->available_at_ms = next_available; //el cooldown le marcamos cuando estará disponible de nuevo
// 	self->right_dongle->taken = false;
// 	pthread_mutex_unlock(&self->right_dongle->mutex);

// 	printf("    - TERMINAR Y SOLTAR DONGLE programador ID-%i\n", self->id);
// 	self->compile_count++;
// 	return (false);
// }

bool	do_debug(t_coder *self)
{
	printf(" - DEBUGGING programador ID-%i\n", self->id);
	usleep(self->config->time_to_debug * 1000);
    log_status(self, "is debugging");
	return (check_burnout(self));
}

bool	do_refactor(t_coder *self)
{
	//REFACTORIZAR
	printf(" - REFACTORIZANDO programador ID-%i\n", self->id);
	usleep(self->config->time_to_refactor * 1000);
    log_status(self, "is refactoring");
	return (check_burnout(self));
}
