/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aitorres <aitorres@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 00:00:00 by aitorres          #+#    #+#             */
/*   Updated: 2026/06/05 13:10:08 by aitorres         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODER_H
# define CODER_H

struct	s_dongle;

struct	s_context;

typedef struct s_coder
{
	int					id;
	int					compile_count;
	long long			last_compile_ms;
	t_dongle			*left_dongle;
	t_dongle			*right_dongle;
	t_config			*config;
	struct s_context	*ctx;
}	t_coder;

t_coder	*create_and_init_coders(int count, t_dongle *dongles,
			struct s_context *ctx);
void	init_coders(t_coder *coders, t_dongle *dongles, int count,
			struct s_context *ctx);
void	*coder_routine(void *arg);

#endif
