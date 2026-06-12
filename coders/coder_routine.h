/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_routine.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aitorres <aitorres@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 00:00:00 by jaitowerr         #+#    #+#             */
/*   Updated: 2026/06/10 16:27:06 by aitorres         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODER_ROUTINE_H
# define CODER_ROUTINE_H

struct t_coder;

void *coder_executed(void *arg);

bool check_burnout(t_coder *self);

bool	compile(t_coder *self);

bool	release_and_cooldown(t_coder *self);

bool	do_debug(t_coder *self);

bool	do_refactor(t_coder *self);

// bool	take_dongles(t_coder *self);

bool	take_dongles_fifo(t_coder *self);

bool	take_dongles_edf(t_coder *self);



#endif