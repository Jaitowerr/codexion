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

#ifndef DONGLE_H
# define DONGLE_H

struct s_context;

typedef struct s_dongle
{
	int             id;	//identificador mismo que coder
	int             coder_id;	// el id del coder al que pertenece
	bool            taken;	//si está siendo usado o no
	struct timeval	available_at;	//fecha/hora en el qiue estará disponible  dongle_cooldown
	pthread_mutex_t	mutex;	//sirve para proteger acceso porque varios programadores pueden intentar cogerlo a la vez
	pthread_cond_t	cond;	//sirve para avisar a los hilos que esperana que esté libre
	// struct s_dongle *next;
	// struct s_dongle *prev;
}					t_dongle;

t_dongle *create_and_init_dongles(int count, struct s_context *ctx);

void				init_dongles(t_dongle *dongles, int count);

#endif