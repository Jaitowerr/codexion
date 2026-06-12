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

// static bool unlock_dongle_mutex(t_coder *self)
// {
// 	self->left_dongle->taken = false;
// 	pthread_mutex_unlock(&self->left_dongle->mutex);
// 	if (self->left_dongle != self->right_dongle)  // Solo si son distintos, caso apra 1 programador
// 	{
// 		self->right_dongle->taken = false;
// 		pthread_mutex_unlock(&self->right_dongle->mutex);
// 	}
// 	return (true);
// }

bool check_burnout(t_coder *self)
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

// bool check_burnout(t_coder *self)
// {
// 	pthread_mutex_lock(&self->ctx->burnout_mutex);
// 	if (self->ctx->someone_burned)
// 	{
// 		pthread_mutex_unlock(&self->ctx->burnout_mutex);
// 		return (true);
// 	}
// 	if (get_current_time_ms() - self->last_compile_ms > self->config->time_to_burnout)
// 	{
// 		self->ctx->someone_burned = true;
// 		log_status(self, "burned out");
// 		// printf("***************BURNOUT programador ID-%i\n", self->id);
// 		pthread_mutex_unlock(&self->ctx->burnout_mutex);
// 		return (true);
// 	}
// 	pthread_mutex_unlock(&self->ctx->burnout_mutex);
// 	return (false);
// }

// ... includes arriba ...

bool	take_dongles_fifo(t_coder *self)
{
	t_dongle	*first;
	t_dongle	*second;

	// Esperar a que ambos dongles estén disponibles
	while (get_current_time_ms() < self->left_dongle->available_at_ms
		|| get_current_time_ms() < self->right_dongle->available_at_ms)
	{
		if (check_burnout(self))
			return (true);
		usleep(100);
	}
	
	if (check_burnout(self))
		return (true);

	// Caso especial: 1 solo programador
	if (self->left_dongle == self->right_dongle)
	{
		pthread_mutex_lock(&self->left_dongle->mutex);
		self->left_dongle->taken = true;
		log_status(self, "has taken a dongle");
		if (check_burnout(self))
		{
			self->left_dongle->taken = false;
			pthread_mutex_unlock(&self->left_dongle->mutex);
			return (true);
		}
		while (!check_burnout(self))
			usleep(1000);
		return (true);
	}

	// Orden pares/impares para evitar deadlock
	if (self->id % 2 == 0)
	{
		first = self->right_dongle;
		second = self->left_dongle;
	}
	else
	{
		first = self->left_dongle;
		second = self->right_dongle;
	}

	// Coger el primer dongle
	pthread_mutex_lock(&first->mutex);
	first->taken = true;
	log_status(self, "has taken a dongle");
	
	// Verificar burnout después del primer dongle
	if (check_burnout(self))
	{
		first->taken = false;
		pthread_mutex_unlock(&first->mutex);
		return (true);
	}

	// Coger el segundo dongle
	pthread_mutex_lock(&second->mutex);
	second->taken = true;
	log_status(self, "has taken a dongle");

	// Verificar burnout después del segundo dongle
	if (check_burnout(self))
	{
		// Liberar ambos dongles si se quema
		first->taken = false;
		pthread_mutex_unlock(&first->mutex);
		second->taken = false;
		pthread_mutex_unlock(&second->mutex);
		return (true);
	}
	
	return (false);
}

// Por ahora EDF usa exactamente la misma lógica que FIFO; luego la cambiaremos
bool	take_dongles_edf(t_coder *self)
{
	return (take_dongles_fifo(self));
}

// bool	take_dongles(t_coder *self)
// {
// 	t_dongle	*first;
// 	t_dongle	*second;

// 	while (get_current_time_ms() < self->left_dongle->available_at_ms
// 		|| get_current_time_ms() < self->right_dongle->available_at_ms)
// 	{
// 		if (check_burnout(self))
// 			return (true);
// 		usleep(100);
// 	}
// 	if (check_burnout(self))
// 		return (true);

// 	// Caso especial: 1 coder (mismo dongle para ambos lados)
// 	if (self->left_dongle == self->right_dongle)
// 	{
// 		pthread_mutex_lock(&self->left_dongle->mutex);
// 		log_status(self, "has taken a dongle");
// 		self->left_dongle->taken = false;
// 		pthread_mutex_unlock(&self->left_dongle->mutex);
// 		while (!check_burnout(self))
// 			usleep(1000);
// 		return (true);
// 	}

// 	// Orden pares e impares para romper el deadlock circular
// 	if (self->id % 2 == 0)
// 	{
// 		first = self->right_dongle;
// 		second = self->left_dongle;
// 	}
// 	else
// 	{
// 		first = self->left_dongle;
// 		second = self->right_dongle;
// 	}

// 	pthread_mutex_lock(&first->mutex);
// 	first->taken = true;
// 	log_status(self, "has taken a dongle");

// 	pthread_mutex_lock(&second->mutex);
// 	second->taken = true;
// 	log_status(self, "has taken a dongle");

// 	if (check_burnout(self))
// 	{
// 		first->taken = false;
// 		pthread_mutex_unlock(&first->mutex);
// 		second->taken = false;
// 		pthread_mutex_unlock(&second->mutex);
// 		return (true);
// 	}
// 	return (false);
// }


bool	compile(t_coder *self)
{
	if (check_burnout(self))
		return (true);
    
	log_status(self, "is compiling");
    
	// pthread_mutex_lock(&self->ctx->burnout_mutex);
	self->last_compile_ms = get_current_time_ms();     // El "time to burnout" se resetea al empezar a compilar.
	// pthread_mutex_unlock(&self->ctx->burnout_mutex);

	// HACEMOS LA ACCIÓN
	usleep(self->config->time_to_compile * 1000);
	
	return (check_burnout(self));
}

// bool	compile(t_coder *self)
// {
// 	//COMPILAR
// 	// printf("   - COMPILAR programador ID-%i\n", self->id);
// 	usleep(self->config->time_to_compile * 1000);
// 	log_status(self, "is compiling");

// 	if (check_burnout(self))
// 	{
// 		// Soltar dongles porque los tenemos cogidos
// 		return (unlock_dongle_mutex(self));
// 	}
// 	return (false);
// }

