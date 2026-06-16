/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aitorres  <aitorres @student.42madrid.com> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 00:00:00 by aitorres          #+#    #+#             */
/*   Updated: 2026/06/04 00:00:00 by aitorres         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DONGLE_H
# define DONGLE_H

struct s_context;

typedef struct s_request
{
    int              coder_id;
	long long        deadline; 
    bool             granted;
    pthread_cond_t   cond;
    struct s_request *next;
}   t_request;

typedef struct s_dongle
{
	int             id;
	int             coder_id;
	bool            taken;
	long			available_at_ms;
	pthread_mutex_t	mutex;
	pthread_cond_t	cond;
	t_request       *wait_queue;
}					t_dongle;

t_dongle 	*create_and_init_dongles(int count, struct s_context *ctx);

void		init_dongles(t_dongle *dongles, int count);

#endif