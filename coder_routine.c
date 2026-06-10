/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_routine.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aitorres <aitorres@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 00:00:00 by jaitowerr         #+#    #+#             */
/*   Updated: 2026/06/10 17:01:47 by aitorres         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders/codexion.h"

static bool unlock_dongle_mutex(t_coder *self)
{
	self->left_dongle->taken = false;
	pthread_mutex_unlock(&self->left_dongle->mutex);
	self->right_dongle->taken = false;
	pthread_mutex_unlock(&self->right_dongle->mutex);
	return true;
}

bool check_burnout(t_coder *self)
{
	pthread_mutex_lock(&self->ctx->burnout_mutex);
	if (self->ctx->someone_burned)
	{
		pthread_mutex_unlock(&self->ctx->burnout_mutex);
		return (true);
	}
	if (get_current_time_ms() - self->last_compile_ms > self->config->time_to_burnout)
	{
		self->ctx->someone_burned = true;
		printf("***************BURNOUT programador ID-%i\n", self->id);
		pthread_mutex_unlock(&self->ctx->burnout_mutex);
		return (true);
	}
	pthread_mutex_unlock(&self->ctx->burnout_mutex);
	return (false);
}

bool	take_dongles(t_coder *self)
{
	while ((get_current_time_ms() < self->left_dongle->available_at_ms)
		|| (get_current_time_ms() < self->right_dongle->available_at_ms))
	{
		if (check_burnout(self))
			return (true);
		usleep(100); 	//Mientras izq o dr no esté disponible, duerme el hilo o espera 0.1 milisegundo, mejor prueba, mas consumo de cpu
	}
	if (check_burnout(self))
		return (true);

	//COGER DONGLES
	pthread_mutex_lock(&self->left_dongle->mutex);
	pthread_mutex_lock(&self->right_dongle->mutex);
								// if dongle is disppoonible
	self->left_dongle->taken = true;		//luego eliminar y de la lista
	self->right_dongle->taken = true;
	if (check_burnout(self))
	{
		// Soltar dongles porque los tenemos cogidos
		return (unlock_dongle_mutex(self));
	}
	printf("  - COGER DONGLES programador ID-%i izq %i dr %i\n",
		self->id, self->left_dongle->id, self->right_dongle->id);
	return (false);
}

bool	compile(t_coder *self)
{
	//COMPILAR
	printf("   - COMPILAR programador ID-%i\n", self->id);
	usleep(self->config->time_to_compile * 1000);
	if (check_burnout(self))
	{
		// Soltar dongles porque los tenemos cogidos
		return (unlock_dongle_mutex(self));
	}
	return (false);
}

