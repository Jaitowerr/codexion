/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor_routine.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aitorres <aitorres@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 00:00:00 by aitorres          #+#    #+#             */
/*   Updated: 2026/06/05 13:04:04 by aitorres         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders/codexion.h"


// Comprueba si la simulación ha parado por burnout
static bool	is_simulation_over(t_context *ctx)
{
	bool	over;

	pthread_mutex_lock(&ctx->burnout_mutex);
	over = ctx->someone_burned;
	pthread_mutex_unlock(&ctx->burnout_mutex);
	return (over);
}

static bool	handle_coder_burnout(t_context *ctx, int idx)
{
	pthread_mutex_lock(&ctx->burnout_mutex);
	if (ctx->someone_burned)
	{
		pthread_mutex_unlock(&ctx->burnout_mutex);
		return (true);
	}
	ctx->someone_burned = true;
	pthread_mutex_unlock(&ctx->burnout_mutex);
	log_status(&ctx->coders[idx], "burned out");
	return (true);
}


static bool	check_all_coders(t_context *ctx, bool *all_done)
{
	int			i;
	long long	now;

	i = 0;
	*all_done = true;
	while (i < ctx->config->number_of_coders)
	{
		/* if this coder finished required compiles, skip burnout check */
		if (ctx->coders[i].compile_count
			>= ctx->config->number_of_compiles_required)
		{
			i++;
			continue ;
		}
		now = get_current_time_ms();
		if (now > ctx->coders[i].last_compile_ms
			+ ctx->config->time_to_burnout)
			return (handle_coder_burnout(ctx, i));
		*all_done = false;
		i++;
	}
	return (false);
}

static void	wake_all_dongles(t_context *ctx)
{
	int			i;
	t_request	*req;

	i = 0;
	while (i < ctx->config->number_of_coders)
	{
		pthread_mutex_lock(&ctx->dongles[i].mutex);
		req = ctx->dongles[i].wait_queue;
		while (req)
		{
			// pthread_cond_broadcast(&req->cond);
			pthread_cond_broadcast(&req->cond);
			req = req->next;
		}
		pthread_mutex_unlock(&ctx->dongles[i].mutex);
		i++;
	}
}

void	*monitor_routine(void *arg)
{
	t_context	*ctx;
	bool		all_done;

	ctx = (t_context *)arg;
	while (1)
	{
		if (is_simulation_over(ctx))
		{
			wake_all_dongles(ctx);
			break ;
		}
		if (check_all_coders(ctx, &all_done))
		{
			wake_all_dongles(ctx);
			break ;
		}
		if (all_done)
			break ;
		usleep(5000);
	}
	return (NULL);
}