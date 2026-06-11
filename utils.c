/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aitorres <aitorres@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 00:00:00 by jaitowerr         #+#    #+#             */
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
    // ctx->start_time = get_current_time_ms();
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

void free_context(t_context *ctx)
{
    if (ctx->config)
    {
    //     free(ctx->config);
        ctx->config = NULL;
    }
    if (ctx->dongles)
    {
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


// void	print_summary(t_context *ctx)
// {
// 	int	i;

// 	i = 0;
// 	while (i < ctx->config->number_of_coders)
// 	{
// 		printf("Coder[%d]:\n", ctx->coders[i].id);
// 		printf("  Left Dongle: %d\n", ctx->coders[i].left_dongle->id);
// 		printf("  Right Dongle: %d\n", ctx->coders[i].right_dongle->id);
// 		i++;
// 	}
// }

// void log_status(t_coder *self, const char *status)
// {
// 	long long current_time;

//     pthread_mutex_lock(&self->ctx->log_mutex);
// 	if (self->ctx->someone_burned)
// 	{
// 		pthread_mutex_unlock(&self->ctx->log_mutex);
// 		return;
// 	}
// 	current_time = get_current_time_ms() - self->ctx->start_time;
// 	printf("%lld %d %s\n", current_time, self->id, status);
// 	pthread_mutex_unlock(&self->ctx->log_mutex);
// }

void	log_status(t_coder *self, const char *status)
{
	t_context	*ctx;
	long long	timestamp;

	ctx = self->ctx;
	// 1. Bloqueamos en el orden jerárquico establecido
	pthread_mutex_lock(&ctx->log_mutex);
	pthread_mutex_lock(&ctx->burnout_mutex);
	
	// 2. Solo imprimimos si nadie se ha quemado todavía
	if (!ctx->someone_burned)
	{
		timestamp = get_current_time_ms() - ctx->start_time;
		printf("%lld %d %s\n", timestamp, self->id, status);
	}
	
	// 3. Desbloqueamos en orden inverso
	pthread_mutex_unlock(&ctx->burnout_mutex);
	pthread_mutex_unlock(&ctx->log_mutex);
}
