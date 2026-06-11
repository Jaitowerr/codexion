/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aitorres <aitorres@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 00:00:00 by jaitowerr         #+#    #+#             */
/*   Updated: 2026/06/05 17:03:36 by aitorres         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders/codexion.h"

int	main(int argc, char **argv)
{
	t_context 	ctx;
	t_config	config;

	
	init_context(&ctx);

	if (!parse_arguments(argc, argv, &config))
	{
		free_context(&ctx);
		exit(1);
	}
	printf("--- Configuración cargada ---\n");
	// printf(" - Número de programadores: %d\n", config.number_of_coders);
	// printf(" - Tiempo para quemarse (ms): %d\n", config.time_to_burnout);
	// printf(" - Tiempo de compilación (ms): %d\n", config.time_to_compile);
	// printf(" - Tiempo de depuración (ms): %d\n", config.time_to_debug);
	// printf(" - Tiempo de refactorización (ms): %d\n", config.time_to_refactor);
	// printf(" - Compilaciones requeridas: %d\n", config.number_of_compiles_required);
	// printf(" - Tiempo de cooldown de dongle (ms): %d\n", config.dongle_cooldown);
	printf(" - Planificador (scheduler): %s\n", config.scheduler);
	printf("--------------------------------\n");


	// ctx.dongles = safe_malloc(sizeof(t_dongle) * config.number_of_coders, &ctx);
    // ctx.coders = safe_malloc(sizeof(t_coder) * config.number_of_coders, &ctx);

	// init_dongles(ctx.dongles, config.number_of_coders);
    // init_coders(ctx.coders, ctx.dongles, config.number_of_coders, &config);

	ctx.config = &config;
	ctx.dongles = create_and_init_dongles(config.number_of_coders, &ctx);
	ctx.coders = create_and_init_coders(config.number_of_coders, ctx.dongles, &config, &ctx);

	// print_summary(&ctx);

	create_threads(&ctx);

	free_context(&ctx);
	return (0);
}
