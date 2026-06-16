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


void	handle_burnout_edf(t_dongle *dongle, t_request *req)
{
	if (req->granted)
	{
		dongle->taken = false;
		if (dongle->wait_queue)
		{
			dongle->wait_queue->granted = true;
			dongle->taken = true;
			pthread_cond_broadcast(&dongle->wait_queue->cond);
		}
	}
}

void	cleanup_on_burnout_both_edf(t_dongle *first, t_dongle *second)
{
	pthread_mutex_lock(&first->mutex);
	first->taken = false;
	pthread_mutex_unlock(&first->mutex);
	pthread_mutex_lock(&second->mutex);
	second->taken = false;
	pthread_mutex_unlock(&second->mutex);
}