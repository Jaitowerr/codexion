/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor_routine.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aitorres <aitorres@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 00:00:00 by aitorres          #+#    #+#             */
/*   Updated: 2026/06/10 18:14:23 by aitorres         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders/codexion.h"


static bool all_finish(t_context *ctx)
{
	int i;

	i = 0;
	while ( i < ctx->config->number_of_coders)
	{
		if (ctx->coders[i].compile_count < ctx->config->number_of_compiles_required)
			return false;
		i++;
	}
	return true;
}

void *monitor_routine(void *arg)
{
    t_context 	*ctx;
	int 		i;
	long long	now;

    ctx = (t_context *)arg;	// casteamos

	while (!ctx->someone_burned && !all_finish(ctx))
	{
		i = 0;
		while (i < ctx->config->number_of_coders)
		{
			now = get_current_time_ms();
			if (now - ctx->coders[i].last_compile_ms > ctx->config->time_to_burnout)
			{
				pthread_mutex_lock(&ctx->burnout_mutex);
				ctx->someone_burned = true;
				log_status(arg, "DIEEEEEEDDDDDDDD!!!!!!!!!!!");
				printf("                         monitor_routine   ***************BURNOUT programador ID-%i\n", ctx->coders[i].id);
				pthread_mutex_unlock(&ctx->burnout_mutex);
				
				// Despertar a todos los que estén esperando en algún dongle
				int j = 0;
				while (j < ctx->config->number_of_coders)
				{
					pthread_mutex_lock(&ctx->dongles[j].mutex);
					pthread_cond_broadcast(&ctx->dongles[j].cond);
					pthread_mutex_unlock(&ctx->dongles[j].mutex);
					j++;
				}
				break;
			}
			i++;
		}
		usleep(500);
	}
	return (NULL);
}