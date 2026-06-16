/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_routine_edf_two.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aitorres <aitorres@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 00:00:00 by aitorres          #+#    #+#             */
/*   Updated: 2026/06/15 18:23:46 by aitorres         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders/codexion.h"

void	init_request_edf(t_request *req, t_coder *self)
{
	req->coder_id = self->id;
	req->deadline = self->last_compile_ms + self->config->time_to_burnout;
	req->granted = false;
	req->next = NULL;
}

void	insert_in_queue_edf(t_dongle *dongle, t_request *req)
{
	t_request	**cur;

	cur = &dongle->wait_queue;
	while (*cur && (*cur)->deadline <= req->deadline)
		cur = &(*cur)->next;
	req->next = *cur;
	*cur = req;
}

void	wait_for_grant_or_burnout_edf(t_dongle *dongle, t_request *req,
	t_coder *self)
{
	while (!req->granted && !check_burnout(self))
		pthread_cond_wait(&req->cond, &dongle->mutex);
}

void	remove_from_queue_edf(t_dongle *dongle, t_request *req)
{
	t_request	**cur;

	cur = &dongle->wait_queue;
	while (*cur && *cur != req)
		cur = &(*cur)->next;
	if (*cur)
		*cur = req->next;
}
