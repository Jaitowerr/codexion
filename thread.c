/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   thread.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aitorres <aitorres@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 00:00:00 by aitorres          #+#    #+#             */
/*   Updated: 2026/06/05 13:08:03 by aitorres         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders/codexion.h"

void	create_threads(t_context *ctx)
{
	int	i;

	ctx->start_time = get_current_time_ms();
	ctx->threads = safe_malloc(sizeof(pthread_t) * ctx->config->numb_of_coders,
			ctx);
	i = 0;
	while (i < ctx->config->numb_of_coders)
	{
		ctx->coders[i].last_compile_ms = ctx->start_time;
		i++;
	}
	i = 0;
	while (i < ctx->config->numb_of_coders)
	{
		pthread_create(&ctx->threads[i], NULL, coder_executed, &ctx->coders[i]);
		i++;
	}
	pthread_create(&ctx->monitor_thread, NULL, monitor_routine, ctx);
	i = 0;
	while (i < ctx->config->numb_of_coders)
	{
		pthread_join(ctx->threads[i], NULL);
		i++;
	}
	pthread_join(ctx->monitor_thread, NULL);
}
