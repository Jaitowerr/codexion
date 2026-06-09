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
}


void	print_summary(t_context *ctx)
{
	int	i;

	i = 0;
	while (i < ctx->config->number_of_coders)
	{
		printf("Coder[%d]:\n", ctx->coders[i].id);
		printf("  Left Dongle: %d\n", ctx->coders[i].left_dongle->id);
		printf("  Right Dongle: %d\n", ctx->coders[i].right_dongle->id);
		i++;
	}
}
