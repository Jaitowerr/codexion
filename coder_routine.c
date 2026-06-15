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

// Funciones auxiliares para take_one_dongle_edf
static void	init_request_edf(t_request *req, t_coder *self)
{
	req->coder_id = self->id;
	req->deadline = self->last_compile_ms + self->config->time_to_burnout;
	req->granted  = false;
	req->next     = NULL;
}

static void	insert_in_queue_edf(t_dongle *dongle, t_request *req)
{
	t_request	**cur;

	cur = &dongle->wait_queue;
	while (*cur && (*cur)->deadline <= req->deadline)
		cur = &(*cur)->next;
	req->next = *cur;
	*cur = req;
}

static void	wait_for_grant_or_burnout_edf(t_dongle *dongle, t_request *req,
	t_coder *self)
{
	while (!req->granted && !check_burnout(self))
		pthread_cond_wait(&req->cond, &dongle->mutex);
}

static void	remove_from_queue_edf(t_dongle *dongle, t_request *req)
{
	t_request	**cur;

	cur = &dongle->wait_queue;
	while (*cur && *cur != req)
		cur = &(*cur)->next;
	if (*cur)
		*cur = req->next;
}

static void	handle_burnout_edf(t_dongle *dongle, t_request *req)
{
	if (req->granted)
	{
		dongle->taken = false;
		if (dongle->wait_queue)
		{
			dongle->wait_queue->granted = true;
			dongle->taken = true;
			pthread_cond_signal(&dongle->wait_queue->cond);
		}
	}
}

static void	wait_cooldown_edf(t_dongle *dongle, t_request *req, t_coder *self)
{
	struct timespec	ts;
	long long		wait_until;

	wait_until = dongle->available_at_ms;
	while (get_current_time_ms() < wait_until && !check_burnout(self))
	{
		ts.tv_sec  = wait_until / 1000;
		ts.tv_nsec = (wait_until % 1000) * 1000000;
		pthread_cond_timedwait(&req->cond, &dongle->mutex, &ts);
	}
}

// Función take_one_dongle_edf refactorizada
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

	wait_cooldown_edf(dongle, &req, self);
	pthread_mutex_unlock(&dongle->mutex);
	pthread_cond_destroy(&req.cond);
}

// Funciones auxiliares para take_dongles_edf
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

static void	cleanup_on_burnout_single_edf(t_dongle *first)
{
	pthread_mutex_lock(&first->mutex);
	first->taken = false;
	pthread_mutex_unlock(&first->mutex);
}

static void	cleanup_on_burnout_both_edf(t_dongle *first, t_dongle *second)
{
	pthread_mutex_lock(&first->mutex);
	first->taken = false;
	pthread_mutex_unlock(&first->mutex);
	pthread_mutex_lock(&second->mutex);
	second->taken = false;
	pthread_mutex_unlock(&second->mutex);
}

// Función take_dongles_edf refactorizada
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

// Funciones auxiliares para take_one_dongle_fifo
static void	init_request_fifo(t_request *req, t_coder *self)
{
	req->coder_id = self->id;
	req->granted  = false;
	req->next     = NULL;
}

static void	insert_in_queue_fifo(t_dongle *dongle, t_request *req)
{
	t_request	**cur;

	cur = &dongle->wait_queue;
	while (*cur)
		cur = &(*cur)->next;
	*cur = req;
}

static void	wait_for_grant_or_burnout_fifo(t_dongle *dongle, t_request *req,
	t_coder *self)
{
	while (!req->granted && !check_burnout(self))
		pthread_cond_wait(&req->cond, &dongle->mutex);
}

static void	remove_from_queue_fifo(t_dongle *dongle, t_request *req)
{
	t_request	**cur;

	cur = &dongle->wait_queue;
	while (*cur && *cur != req)
		cur = &(*cur)->next;
	if (*cur)
		*cur = req->next;
}

static void	handle_burnout_fifo(t_dongle *dongle, t_request *req)
{
	if (req->granted)
	{
		dongle->taken = false;
		if (dongle->wait_queue)
		{
			dongle->wait_queue->granted = true;
			dongle->taken = true;
			pthread_cond_signal(&dongle->wait_queue->cond);
		}
	}
}

static void	wait_cooldown_fifo(t_dongle *dongle, t_request *req, t_coder *self)
{
	struct timespec	ts;
	long long		wait_until;

	wait_until = dongle->available_at_ms;
	while (get_current_time_ms() < wait_until && !check_burnout(self))
	{
		ts.tv_sec  = wait_until / 1000;
		ts.tv_nsec = (wait_until % 1000) * 1000000;
		pthread_cond_timedwait(&req->cond, &dongle->mutex, &ts);
	}
}

// Función take_one_dongle_fifo refactorizada
static void	take_one_dongle_fifo(t_dongle *dongle, t_coder *self)
{
	t_request	req;

	init_request_fifo(&req, self);
	pthread_cond_init(&req.cond, NULL);
	pthread_mutex_lock(&dongle->mutex);

	if (!dongle->taken && get_current_time_ms() >= dongle->available_at_ms)
	{
		dongle->taken = true;
		pthread_mutex_unlock(&dongle->mutex);
		pthread_cond_destroy(&req.cond);
		return ;
	}

	insert_in_queue_fifo(dongle, &req);
	wait_for_grant_or_burnout_fifo(dongle, &req, self);
	remove_from_queue_fifo(dongle, &req);

	if (check_burnout(self))
	{
		handle_burnout_fifo(dongle, &req);
		pthread_mutex_unlock(&dongle->mutex);
		pthread_cond_destroy(&req.cond);
		return ;
	}

	wait_cooldown_fifo(dongle, &req, self);
	pthread_mutex_unlock(&dongle->mutex);
	pthread_cond_destroy(&req.cond);
}

// Funciones auxiliares para take_dongles_fifo
static void	get_dongle_order_fifo(t_coder *self, t_dongle **first, t_dongle **second)
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

static void	cleanup_on_burnout_single_fifo(t_dongle *first)
{
	pthread_mutex_lock(&first->mutex);
	first->taken = false;
	pthread_mutex_unlock(&first->mutex);
}

static void	cleanup_on_burnout_both_fifo(t_dongle *first, t_dongle *second)
{
	pthread_mutex_lock(&first->mutex);
	first->taken = false;
	pthread_mutex_unlock(&first->mutex);
	pthread_mutex_lock(&second->mutex);
	second->taken = false;
	pthread_mutex_unlock(&second->mutex);
}

// Función take_dongles_fifo refactorizada
bool	take_dongles_fifo(t_coder *self)
{
	t_dongle	*first;
	t_dongle	*second;

	if (check_burnout(self))
		return (true);

	if (self->left_dongle == self->right_dongle)
	{
		take_one_dongle_fifo(self->left_dongle, self);
		log_status(self, "has taken a dongle");
		while (!check_burnout(self))
			usleep(1000);
		return (true);
	}

	get_dongle_order_fifo(self, &first, &second);
	take_one_dongle_fifo(first, self);

	if (check_burnout(self))
	{
		cleanup_on_burnout_single_fifo(first);
		return (true);
	}
	log_status(self, "has taken a dongle");

	take_one_dongle_fifo(second, self);
	if (check_burnout(self))
	{
		cleanup_on_burnout_both_fifo(first, second);
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
