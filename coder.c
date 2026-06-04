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

void init_coders(t_coder *coders, t_dongle *dongles, int count, t_config *config)
{
    int i;

    i = 0;
    while (i < count)
    {
        coders[i].id = i;
        coders[i].compile_count = 0;
        coders[i].last_compile_start.tv_sec = 0;
        coders[i].last_compile_start.tv_usec = 0;
        coders[i].left_dongle = &dongles[i];
        coders[i].right_dongle = &dongles[(i + 1) % count];
        coders[i].config = config;
        i++;
    }
}

t_coder *create_and_init_coders(int count, t_dongle *dongles, t_config *config, t_context *ctx)
{
    t_coder *coders;

    coders = safe_malloc(sizeof(t_coder) * count, ctx);
    init_coders(coders, dongles, count, config);
    return (coders);
}


// void create(t_coder  *coders)
// {
//     coders = malloc(sizeof(t_coder) * config.number_of_coders)
//     if (!coders)
//     {
//         printf("Error: No se pudo reservar memoria.\n");
//         return 1;
//     }
// }