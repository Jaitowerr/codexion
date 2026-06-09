/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_routine.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aitorres <aitorres@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 00:00:00 by jaitowerr         #+#    #+#             */
/*   Updated: 2026/06/09 19:23:26 by aitorres         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODER_ROUTINE_H
# define CODER_ROUTINE_H

struct t_coder;

void *coder_executed(void *arg);

bool check_burnout(t_coder *self);



#endif