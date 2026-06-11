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
		printf("descubiertos:\n");
		printf("	***descubierto id: %i\n", self->id);
		pthread_mutex_unlock(&self->ctx->burnout_mutex);
		return (true);
	}
	pthread_mutex_unlock(&self->ctx->burnout_mutex);
	return (false);
}

static long long get_priority(t_coder *self)
{
	if (strcmp(self->config->scheduler, "edf") == 0)
	{
		// Deadline = inicio última compilación + tiempo para burnout
		return (self->last_compile_ms + self->config->time_to_burnout);
	}
	// Si es FIFO, la prioridad es el momento de la petición
	return (get_current_time_ms());
}


bool take_dongles(t_coder *self)
{
    long long   priority;
    t_dongle    *first;
    t_dongle    *second;

    priority = get_priority(self);
    self->request_ms = priority;

    // Siempre bloqueamos primero el de ID más bajo para evitar deadlock
    if (self->left_dongle->id < self->right_dongle->id)
    {
        first = self->left_dongle;
        second = self->right_dongle;
    }
    else
    {
        first = self->right_dongle;
        second = self->left_dongle;
    }

    // --- Coger el primer dongle ---
    pthread_mutex_lock(&first->mutex);
    enqueue_waiter(first, self->id, priority);
    while (first->taken || first->wait_queue->coder_id != self->id
        || get_current_time_ms() < first->available_at_ms)
    {
        if (check_burnout(self))
        {
            dequeue_waiter(first, self->id);
            pthread_mutex_unlock(&first->mutex);
            return (true);
        }
        pthread_cond_wait(&first->cond, &first->mutex);
    }
    dequeue_waiter(first, self->id);
    first->taken = true;
    pthread_mutex_unlock(&first->mutex);

    // --- Coger el segundo dongle ---
    pthread_mutex_lock(&second->mutex);
    enqueue_waiter(second, self->id, priority);
    while (second->taken || second->wait_queue->coder_id != self->id
        || get_current_time_ms() < second->available_at_ms)
    {
        if (check_burnout(self))
        {
            dequeue_waiter(second, self->id);
            pthread_mutex_unlock(&second->mutex);
            first->taken = false;
            pthread_cond_broadcast(&first->cond);
            return (true);
        }
        pthread_cond_wait(&second->cond, &second->mutex);
    }
    dequeue_waiter(second, self->id);
    second->taken = true;
    pthread_mutex_unlock(&second->mutex);

    if (check_burnout(self))
    {
        first->taken = false;
        pthread_cond_broadcast(&first->cond);
        second->taken = false;
        pthread_cond_broadcast(&second->cond);
        return (true);
    }
    return (false);
}


// bool	take_dongles(t_coder *self)
// {
// 	while ((get_current_time_ms() < self->left_dongle->available_at_ms)
// 		|| (get_current_time_ms() < self->right_dongle->available_at_ms))
// 	{
// 		if (check_burnout(self))
// 			return (true);
// 		usleep(100); 	//Mientras izq o dr no esté disponible, duerme el hilo o espera 0.1 milisegundo, mejor prueba, mas consumo de cpu
// 	}
// 	if (check_burnout(self))
// 		return (true);

// 	//COGER DONGLES
// 	if (self->left_dongle->id < self->right_dongle->id)
// 	{
// 		pthread_mutex_lock(&self->left_dongle->mutex);
// 		pthread_mutex_lock(&self->right_dongle->mutex);
// 	}
// 	else
// 	{
// 		pthread_mutex_lock(&self->right_dongle->mutex);
// 		pthread_mutex_lock(&self->left_dongle->mutex);
// 	}
// 								// if dongle is disppoonible
// 	self->left_dongle->taken = true;		//luego eliminar y de la lista
// 	self->right_dongle->taken = true;
// 	if (check_burnout(self))
// 	{
// 		// Soltar dongles porque los tenemos cogidos
// 		return (unlock_dongle_mutex(self));
// 	}
// 	printf("  - COGER DONGLES programador ID-%i izq %i dr %i\n",
// 		self->id, self->left_dongle->id, self->right_dongle->id);
// 	return (false);
// }





bool	compile(t_coder *self)
{
	//COMPILAR
	printf("   - COMPILAR programador ID-%i\n", self->id);
	usleep(self->config->time_to_compile * 1000);
    log_status(self, "is compiling");
	if (check_burnout(self))
	{
		// Soltar dongles porque los tenemos cogidos
		return (unlock_dongle_mutex(self));
	}
	return (false);
}

