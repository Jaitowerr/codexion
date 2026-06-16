/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aitorres <aitorres@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 00:00:00 by aitorres          #+#    #+#             */
/*   Updated: 2026/06/05 17:36:28 by aitorres         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders/codexion.h"

void	init_coders(t_coder *coders, t_dongle *dongles, int count,
		t_context *ctx)
{
	int									i;

	i = 0;
	while (i < count)
	{
		coders[i].id = i + 1;
		coders[i].compile_count = 0;
		coders[i].last_compile_ms = 0;
		coders[i].left_dongle = &dongles[i];
		coders[i].right_dongle = &dongles[(i + 1) % count];
		coders[i].config = ctx->config;
		coders[i].ctx = ctx;
		i++;
	}
}

t_coder	*create_and_init_coders(int count, t_dongle *dongles,
		t_context *ctx)
{
	t_coder	*coders;

	coders = safe_malloc(sizeof(t_coder) * count, ctx);
	init_coders(coders, dongles, count, ctx);
	return (coders);
}

void	*coder_routine(void *arg)
{
	t_coder	*self;

	self = (t_coder *)arg;
	printf("Programador %d: ¡Listo!!! A quemar CPU!!\n", self->id);
	return (NULL);
}
