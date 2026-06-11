/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   thread.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aitorres <aitorres@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 00:00:00 by jaitowerr         #+#    #+#             */
/*   Updated: 2026/06/05 13:08:03 by aitorres         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders/codexion.h"

void create_threads(t_context *ctx)
{
    int i;
    
    ctx->start_time = get_current_time_ms();

    ctx->threads = safe_malloc(sizeof(pthread_t) * ctx->config->number_of_coders, ctx);

    i = 0;
    while (i < ctx->config->number_of_coders)
    {
        pthread_create(&ctx->threads[i], NULL, coder_executed, &ctx->coders[i]); //Crea un nuevo hilo de ejecución. &ctx->threads[i] → Donde se guarda el ID del hilo (para luego hacer join) NULL → Atributos por defecto coder_routine → La función que ejecutará el hilo &ctx->coders[i] → El argumento que le pasamos (el programador)
        i++;
    }

    i = 0;
    while (i < ctx->config->number_of_coders)
    {
        pthread_join(ctx->threads[i], NULL); //Espera a que un hilo termine su ejecución. El main se queda esperando hasta que el programador i termine su trabajo.
        i++;
    }
}
