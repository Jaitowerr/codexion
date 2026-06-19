/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aitorres  <aitorres @student.42madrid.com> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 00:00:00 by aitorres          #+#    #+#             */
/*   Updated: 2026/06/04 00:00:00 by aitorres         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders/codexion.h"

void	init_dongles(t_dongle *dongle, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		dongle[i].id = i + 1;
		dongle[i].coder_id = i + 1;
		dongle[i].taken = false;
		dongle[i].available_at_ms = 0;
		pthread_mutex_init(&dongle[i].mutex, NULL);
		pthread_cond_init(&dongle[i].cond, NULL);
		dongle[i].wait_queue = NULL;
		i++;
	}
}

t_dongle	*create_and_init_dongles(int count, t_context *ctx)
{
	t_dongle	*dongles;

	dongles = safe_malloc(sizeof(t_dongle) * count, ctx);
	init_dongles(dongles, count);
	return (dongles);
}
