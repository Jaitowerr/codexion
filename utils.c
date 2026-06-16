/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aitorres <aitorres@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 00:00:00 by aitorres          #+#    #+#             */
/*   Updated: 2026/06/05 13:04:04 by aitorres         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders/codexion.h"

void init_context(t_context *ctx)
{
    ctx->config = NULL;
    ctx->dongles = NULL;
    ctx->coders = NULL;
    ctx->threads = NULL;
	ctx->someone_burned = false;
    pthread_mutex_init(&ctx->burnout_mutex, NULL);
    pthread_mutex_init(&ctx->log_mutex, NULL);

}

void *safe_malloc(size_t size, t_context *ctx)
{
    void *ptr;

    ptr = malloc(size);
    if (!ptr)
    {
        printf("Error: No se pudo reservar memoria.\n");
        free_context(ctx);
        exit(1);
    }
    return ptr;
}

void	free_context(t_context *ctx)
{
	int	i;

	if (ctx->dongles && ctx->config)
	{
		i = -1;
		while (++i < ctx->config->number_of_coders)
		{
			pthread_mutex_destroy(&ctx->dongles[i].mutex);
			pthread_cond_destroy(&ctx->dongles[i].cond);
		}
		free(ctx->dongles);
		ctx->dongles = NULL;
	}
	if (ctx->coders)
	{
		free(ctx->coders);
		ctx->coders = NULL;
	}
	if (ctx->threads)
	{
		free(ctx->threads);
		ctx->threads = NULL;
	}
	pthread_mutex_destroy(&ctx->burnout_mutex);
	pthread_mutex_destroy(&ctx->log_mutex);
}


void    log_status(t_coder *self, const char *status)
{
    t_context   *ctx;
    long long   timestamp;

    ctx = self->ctx;
    timestamp = get_current_time_ms() - ctx->start_time;

    pthread_mutex_lock(&ctx->log_mutex);
    if (!ctx->someone_burned || strcmp(status, "burned out") == 0)
    {
        printf("%lld %d %s\n", timestamp, self->id, status);
    }
    pthread_mutex_unlock(&ctx->log_mutex);
}
