/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaitowerr <jaitowerr@student.42madrid.com> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 00:00:00 by jaitowerr         #+#    #+#             */
/*   Updated: 2026/06/04 00:00:00 by jaitowerr        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders/codexion.h"

void init_context(t_context *ctx)
{
    ctx->config = NULL;
    ctx->dongles = NULL;
    ctx->coders = NULL;
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