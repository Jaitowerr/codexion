/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_routine.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aitorres <aitorres@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 00:00:00 by jaitowerr         #+#    #+#             */
/*   Updated: 2026/06/15 18:18:41 by aitorres         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders/codexion.h"

static void	take_one_dongle_edf(t_dongle *dongle, t_coder *self)
{
	t_request		req;
	t_request		**cur;
	struct timespec	ts;
	long long		wait_until;

	req.coder_id = self->id;
	req.deadline = self->last_compile_ms + self->config->time_to_burnout;
	req.granted  = false;
	req.next     = NULL;
	pthread_cond_init(&req.cond, NULL);

	pthread_mutex_lock(&dongle->mutex);

	if (!dongle->taken && get_current_time_ms() >= dongle->available_at_ms)
	{
		dongle->taken = true;
		pthread_mutex_unlock(&dongle->mutex);
		pthread_cond_destroy(&req.cond);
		return ;
	}

	// Inserción ordenada por deadline ascendente
	cur = &dongle->wait_queue;
	while (*cur && (*cur)->deadline <= req.deadline)
		cur = &(*cur)->next;
	req.next = *cur;
	*cur = &req;

	// Esperamos hasta que nos concedan el dongle o haya burnout
	while (!req.granted && !check_burnout(self))
		pthread_cond_wait(&req.cond, &dongle->mutex);

	// Nos quitamos de la cola
	cur = &dongle->wait_queue;
	while (*cur && *cur != &req)
		cur = &(*cur)->next;
	if (*cur)
		*cur = req.next;

	// Si hay burnout, devolvemos el dongle si nos lo habían concedido
	if (check_burnout(self))
	{
		if (req.granted)
		{
			dongle->taken = false;
			if (dongle->wait_queue)
			{
				dongle->wait_queue->granted = true;
				dongle->taken = true;
				pthread_cond_signal(&dongle->wait_queue->cond);
			}
		}
		pthread_mutex_unlock(&dongle->mutex);
		pthread_cond_destroy(&req.cond);
		return ;
	}

	// Esperamos el cooldown dentro del mutex con timedwait
	wait_until = dongle->available_at_ms;
	while (get_current_time_ms() < wait_until && !check_burnout(self))
	{
		ts.tv_sec  = wait_until / 1000;
		ts.tv_nsec = (wait_until % 1000) * 1000000;
		pthread_cond_timedwait(&req.cond, &dongle->mutex, &ts);
	}

	pthread_mutex_unlock(&dongle->mutex);
	pthread_cond_destroy(&req.cond);
}

bool	take_dongles_edf(t_coder *self)
{
	t_dongle	*first;
	t_dongle	*second;

	if (check_burnout(self))
		return (true);

	// Caso especial: 1 solo coder
	if (self->left_dongle == self->right_dongle)
	{
		take_one_dongle_edf(self->left_dongle, self);
		log_status(self, "has taken a dongle");
		while (!check_burnout(self))
			usleep(1000);
		return (true);
	}

	if (self->id % 2 == 0)
	{
		first  = self->right_dongle;
		second = self->left_dongle;
	}
	else
	{
		first  = self->left_dongle;
		second = self->right_dongle;
	}

	take_one_dongle_edf(first, self);
	if (check_burnout(self))
	{
		pthread_mutex_lock(&first->mutex);
		first->taken = false;
		pthread_mutex_unlock(&first->mutex);
		return (true);
	}
	log_status(self, "has taken a dongle");

	take_one_dongle_edf(second, self);
	if (check_burnout(self))
	{
		pthread_mutex_lock(&first->mutex);
		first->taken = false;
		pthread_mutex_unlock(&first->mutex);
		pthread_mutex_lock(&second->mutex);
		second->taken = false;
		pthread_mutex_unlock(&second->mutex);
		return (true);
	}
	log_status(self, "has taken a dongle");

	return (false);
}

static void	take_one_dongle_fifo(t_dongle *dongle, t_coder *self)
{
	t_request		req;
	t_request		**cur;
	struct timespec	ts;
	long long		wait_until;

	req.coder_id = self->id;
	req.granted  = false;
	req.next     = NULL;
	pthread_cond_init(&req.cond, NULL);

	pthread_mutex_lock(&dongle->mutex);

	if (!dongle->taken && get_current_time_ms() >= dongle->available_at_ms)
	{
		dongle->taken = true;
		pthread_mutex_unlock(&dongle->mutex);
		pthread_cond_destroy(&req.cond);
		return ;
	}

	// Nos ponemos al final de la cola
	cur = &dongle->wait_queue;
	while (*cur)
		cur = &(*cur)->next;
	*cur = &req;

	// Esperamos hasta que nos concedan el dongle o haya burnout
	while (!req.granted && !check_burnout(self))
		pthread_cond_wait(&req.cond, &dongle->mutex);

	// Nos quitamos de la cola
	cur = &dongle->wait_queue;
	while (*cur && *cur != &req)
		cur = &(*cur)->next;
	if (*cur)
		*cur = req.next;

	// Si hay burnout, devolvemos el dongle si nos lo habían concedido
	if (check_burnout(self))
	{
		if (req.granted)
		{
			dongle->taken = false;
			if (dongle->wait_queue)
			{
				dongle->wait_queue->granted = true;
				dongle->taken = true;
				pthread_cond_signal(&dongle->wait_queue->cond);
			}
		}
		pthread_mutex_unlock(&dongle->mutex);
		pthread_cond_destroy(&req.cond);
		return ;
	}

	// Esperamos el cooldown dentro del mutex con timedwait
	wait_until = dongle->available_at_ms;
	while (get_current_time_ms() < wait_until && !check_burnout(self))
	{
		ts.tv_sec  = wait_until / 1000;
		ts.tv_nsec = (wait_until % 1000) * 1000000;
		pthread_cond_timedwait(&req.cond, &dongle->mutex, &ts);
	}

	pthread_mutex_unlock(&dongle->mutex);
	pthread_cond_destroy(&req.cond);
}

bool	take_dongles_fifo(t_coder *self)
{
	t_dongle	*first;
	t_dongle	*second;

	if (check_burnout(self))
		return (true);
	// Caso especial: 1 solo coder
	if (self->left_dongle == self->right_dongle)
	{

		take_one_dongle_fifo(self->left_dongle, self);
		log_status(self, "has taken a dongle");
		while (!check_burnout(self))
			usleep(1000);
		return (true);
	}

	if (self->id % 2 == 0)
	{
		first  = self->right_dongle;
		second = self->left_dongle;
	}
	else
	{
		first  = self->left_dongle;
		second = self->right_dongle;
	}
	// printf("[DEBUG] coder %d intentando first=%d second=%d\n", self->id, first->id, second->id);

	take_one_dongle_fifo(first, self);
	// printf("[DEBUG] coder %d cogió dongle %d\n", self->id, first->id);
	if (check_burnout(self))
	{
		pthread_mutex_lock(&first->mutex);
		first->taken = false;
		pthread_mutex_unlock(&first->mutex);
		return (true);
	}

	log_status(self, "has taken a dongle");

	take_one_dongle_fifo(second, self);
	// printf("[DEBUG] coder %d cogió dongle %d\n", self->id, second->id);

	if (check_burnout(self))
	{
		pthread_mutex_lock(&first->mutex);
		first->taken = false;
		pthread_mutex_unlock(&first->mutex);
		pthread_mutex_lock(&second->mutex);
		second->taken = false;
		pthread_mutex_unlock(&second->mutex);
		return (true);
	}
	log_status(self, "has taken a dongle");

	return (false);
}

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
