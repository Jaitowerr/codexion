/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaitowerr <jaitowerr@student.42madrid.com> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 00:00:00 by jaitowerr         #+#    #+#             */
/*   Updated: 2026/06/04 00:00:00 by jaitowerr        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders/codexion.h"

int	main(int argc, char **argv)
{
	t_config	config;

	if (!parse_arguments(argc, argv, &config))
		return (1);
	printf("--- Configuración cargada ---\n");
	printf(" - Número de programadores: %d\n", config.number_of_coders);
	printf(" - Tiempo para quemarse (ms): %d\n", config.time_to_burnout);
	printf(" - Tiempo de compilación (ms): %d\n", config.time_to_compile);
	printf(" - Tiempo de depuración (ms): %d\n", config.time_to_debug);
	printf(" - Tiempo de refactorización (ms): %d\n", config.time_to_refactor);
	printf(" - Compilaciones requeridas: %d\n", config.number_of_compiles_required);
	printf(" - Tiempo de cooldown de dongle (ms): %d\n", config.dongle_cooldown);
	printf(" - Planificador (scheduler): %s\n", config.scheduler);
	printf("--------------------------------\n");
	return (0);
}
