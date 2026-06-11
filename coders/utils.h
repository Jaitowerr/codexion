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

#ifndef UTILS_H
# define UTILS_H

# include "codexion.h"
# include <stdlib.h>
# include <stdio.h>

/* Contexto: guarda punteros a todo lo que reservemos */
typedef struct s_context
{
    t_config    	*config;
    t_dongle    	*dongles;
    t_coder     	*coders;
    pthread_t  		*threads;    //pthread_t es un tipo especial de variable que identifica a cada hilo.
    bool			someone_burned;
	pthread_mutex_t burnout_mutex;
	pthread_t 		monitor_thread;	///hilo para el burnout

    long long       start_time;

} t_context;

/* Inicializa el contexto a NULL */
void init_context(t_context *ctx);

/* Libera todo lo que haya en el contexto */
void free_context(t_context *ctx);

/* malloc seguro: si falla, libera y sale */
void *safe_malloc(size_t size, t_context *ctx);

// void	print_summary(t_context *ctx);

void    log_status(t_coder *self, const char *status);


#endif
