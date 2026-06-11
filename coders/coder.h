/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aitorres <aitorres@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 00:00:00 by jaitowerr         #+#    #+#             */
/*   Updated: 2026/06/05 13:10:08 by aitorres         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODER_H
# define CODER_H

// struct s_context;
typedef struct s_context t_context;
struct s_dongle;

typedef struct s_coder
{
	int 			id;            	//id de programador
	int 			compile_count; 	//contador de compilaciones totales
	// struct timeval	last_compile_start;
	long long		last_compile_ms;//empieza en 0
									//momento en que empieza a compilar para calcular tiempo
	t_dongle 		*left_dongle;  	//puntero izquierdo a su dongle
	t_dongle 		*right_dongle; 	//puntero derecho al dongle del vecino
	t_config		*config;		// acceso a a todos lso tiempos y aprámetros globales
	// struct s_coder *next;
	// struct s_coder *prev;
	t_context   	*ctx;
	long long 		request_ms;			//cuándo empezó a esperar dongles
	long long		last_compile_start_ms;//cuándo empezó su última compilación real
}	t_coder;

t_coder *create_and_init_coders(int count, t_dongle *dongles, t_config *config, t_context *ctx);

void	init_coders(t_coder *coders, t_dongle *dongles, int count, t_config *config, t_context *ctx);

void 	*coder_routine(void *arg);

#endif