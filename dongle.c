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

bool enqueue_waiter(t_dongle *dongle, int coder_id, long long priority)
{
    t_waiter *new_node;
    t_waiter **curr;

    new_node = malloc(sizeof(t_waiter));
    if (!new_node)
        return (false);  // fallo, el caller decide qué hacer
    new_node->coder_id = coder_id;
    new_node->priority = priority;
    new_node->next = NULL;

    curr = &dongle->wait_queue;
    while (*curr && (*curr)->priority <= priority)
        curr = &(*curr)->next;
    new_node->next = *curr;
    *curr = new_node;
    return (true);  // éxito
}

void dequeue_waiter(t_dongle *dongle, int coder_id)
{
    t_waiter **curr;
    t_waiter *temp;

    curr = &dongle->wait_queue;
    while (*curr)
    {
        if ((*curr)->coder_id == coder_id)
        {
            temp = *curr;
            *curr = (*curr)->next;
            free(temp);
            return;
        }
        curr = &(*curr)->next;
    }
}

void    init_dongles(t_dongle *dongle, int count)
{
    int i;

    i = 0;
    while (i < count)
    {
        dongle[i].id = i + 1;
        dongle[i].coder_id = i + 1;
        dongle[i].taken = false;
        dongle[i].available_at_ms = 0;
        // gettimeofday(&dongle[i].available_at_ms, NULL);
        pthread_mutex_init(&dongle[i].mutex, NULL);
        pthread_cond_init(&dongle[i].cond, NULL);
        dongle[i].wait_queue = NULL;
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