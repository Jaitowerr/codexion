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


void    init_dongles(t_dongle *dongle, int count)
{
    int i;

    i = 0;
    while (i < count)
    {
        dongle[i].id = i;
        dongle[i].coder_id = i;
        dongle[i].taken = false;
        gettimeofday(&dongle[i].available_at, NULL);
        pthread_mutex_init(&dongle[i].mutex, NULL);
        pthread_cond_init(&dongle[i].cond, NULL);
        i++;
    }
}

t_dongle *create_and_init_dongles(int count, t_context *ctx)
{
    t_dongle *dongles;

    dongles = safe_malloc(sizeof(t_dongle) * count, ctx);
    init_dongles(dongles, count);
    return (dongles);
}