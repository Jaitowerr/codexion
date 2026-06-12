/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aitorres <aitorres@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 00:00:00 by jaitowerr         #+#    #+#             */
/*   Updated: 2026/06/05 13:04:04 by aitorres         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders/codexion.h"


// Comprueba si la simulación ha parado por burnout
static bool	is_simulation_over(t_context *ctx)
{
	bool	over;

	pthread_mutex_lock(&ctx->burnout_mutex);
	over = ctx->someone_burned;
	pthread_mutex_unlock(&ctx->burnout_mutex);
	return (over);
}

// Revisa si un coder se ha quemado o si ha terminado su trabajo
static bool	check_all_coders(t_context *ctx, bool *all_done)
{
	int			i;
	long long	now;

	i = 0;
	*all_done = true;
	while (i < ctx->config->number_of_coders)
	{
		// Si ya terminó, no lo revisamos para burnout, pero marcamos que no todos han acabado si falta alguien
		if (ctx->coders[i].compile_count >= ctx->config->number_of_compiles_required)
		{
			i++;
			continue;
		}

		now = get_current_time_ms();
		if (now > ctx->coders[i].last_compile_ms + ctx->config->time_to_burnout)
		{
			pthread_mutex_lock(&ctx->burnout_mutex);
			if (ctx->someone_burned)
				return (pthread_mutex_unlock(&ctx->burnout_mutex), true);
			ctx->someone_burned = true;
			pthread_mutex_unlock(&ctx->burnout_mutex);
			log_status(&ctx->coders[i], "burned out");
			return (true);
		}
		
		// Si llegamos aquí, es que este coder aún no ha terminado
		*all_done = false;
		i++;
	}
	return (false);
}

// // Revisa si un coder se ha quemado o si ha terminado su trabajo
// static bool	check_all_coders(t_context *ctx, bool *all_done)
// {
// 	int			i;
// 	long long	now;

// 	i = 0;
// 	*all_done = true;
// 	while (i < ctx->config->number_of_coders)
// 	{
// 		now = get_current_time_ms();
// 		if (now > ctx->coders[i].last_compile_ms + ctx->config->time_to_burnout)
// 		{
// 			pthread_mutex_lock(&ctx->burnout_mutex);
// 			if (ctx->someone_burned)
// 				return (pthread_mutex_unlock(&ctx->burnout_mutex), true);
// 			ctx->someone_burned = true;
// 			pthread_mutex_unlock(&ctx->burnout_mutex);
// 			log_status(&ctx->coders[i], "burned out");
// 			return (true);
// 		}
// 		if (ctx->coders[i].compile_count < ctx->config->number_of_compiles_required)
// 			*all_done = false;
// 		i++;
// 	}
// 	return (false);
// }

// Hilo principal de vigilancia
void	*monitor_routine(void *arg)
{
	t_context	*ctx;
	bool		all_done;

	ctx = (t_context *)arg;
	while (1)
	{
		if (is_simulation_over(ctx))              // Si alguien se quemó, sale
			break ;
		if (check_all_coders(ctx, &all_done))      // Si detecta burnout en este ciclo
			break ;
		if (all_done)                             // Si todos terminaron OK
			break ;
		usleep(5000);                             // Precisión de 5ms
	}
	return (NULL);
}


// void	*monitor_routine(void *arg)
// {
// 	t_context	*ctx;
// 	int			i;
// 	long long	now;
// 	long long	deadline;

// 	ctx = (t_context *)arg;
// 	while (1)
// 	{
// 		pthread_mutex_lock(&ctx->burnout_mutex);
// 		if (ctx->someone_burned)
// 		{
// 			pthread_mutex_unlock(&ctx->burnout_mutex);
// 			break ;
// 		}
// 		pthread_mutex_unlock(&ctx->burnout_mutex);

// 		i = 0;
// 		while (i < ctx->config->number_of_coders)
// 		{
// 			now = get_current_time_ms();
// 			deadline = ctx->coders[i].last_compile_ms + ctx->config->time_to_burnout;

// 			if (now > deadline)
// 			{
// 				pthread_mutex_lock(&ctx->burnout_mutex);
// 				if (!ctx->someone_burned) // Doble check para no loguear dos veces
// 				{
// 					ctx->someone_burned = true;
// 					pthread_mutex_unlock(&ctx->burnout_mutex);
// 					log_status(&ctx->coders[i], "burned out");
// 					return (NULL);
// 				}
// 				pthread_mutex_unlock(&ctx->burnout_mutex);
// 			}
// 			i++;
// 		}
// 		usleep(5000); // Revisamos cada 5ms (precisión < 10ms como pide el subject)
// 	}
// 	return (NULL);
// }


// void	*monitor_routine(void *arg)
// {
// 	t_context	*ctx;
// 	int			i;

// 	ctx = (t_context *)arg;
// 	// El monitor corre mientras nadie se haya quemado
// 	while (1)
// 	{
// 		pthread_mutex_lock(&ctx->burnout_mutex);
// 		if (ctx->someone_burned)
// 		{
// 			pthread_mutex_unlock(&ctx->burnout_mutex);
// 			break ;
// 		}
// 		pthread_mutex_unlock(&ctx->burnout_mutex);
		
// 		i = 0;
// 		while (i < ctx->config->number_of_coders)
// 		{
// 			// Si el tiempo actual menos el inicio de su última tarea supera el límite...
// 			if (get_current_time_ms() - ctx->coders[i].last_compile_ms > ctx->config->time_to_burnout)
// 			{
// 				pthread_mutex_lock(&ctx->burnout_mutex);
// 				ctx->someone_burned = true; // El monitor da la orden de parada
// 				pthread_mutex_unlock(&ctx->burnout_mutex);
// 				log_status(&ctx->coders[i], "burned out");
// 				return (NULL);
// 			}
// 			i++;
// 		}
// 		usleep(5000); // Pequeño descanso para no saturar la CPU (5ms)
// 	}
// 	return (NULL);
// }
