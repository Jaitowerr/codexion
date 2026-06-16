/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aitorres  <aitorres @student.42madrid.com> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 00:00:00 by aitorres          #+#    #+#             */
/*   Updated: 2026/06/04 00:00:00 by aitorres         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_H
# define UTILS_H

# include "codexion.h"
# include <stdlib.h>
# include <stdio.h>

typedef struct s_context
{
    t_config    	*config;
    t_dongle    	*dongles;
    t_coder     	*coders;
    pthread_t  		*threads;
    bool			someone_burned;
	pthread_mutex_t burnout_mutex;
	pthread_t 		monitor_thread;
    long long       start_time;
    pthread_mutex_t log_mutex;

} t_context;

void init_context(t_context *ctx);

void free_context(t_context *ctx);

void *safe_malloc(size_t size, t_context *ctx);

void    log_status(t_coder *self, const char *status);


#endif
