/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aitorres <aitorres@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 00:00:00 by aitorres          #+#    #+#             */
/*   Updated: 2026/06/05 17:03:36 by aitorres         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders/codexion.h"

int	main(int argc, char **argv)
{
	t_context	ctx;
	t_config	config;
	int			i;

	init_context(&ctx);
	if (!parse_arguments(argc, argv, &config))
	{
		free_context(&ctx);
		exit(1);
	}
	ctx.config = &config;
	ctx.dongles = create_and_init_dongles(config.numb_of_coders, &ctx);
	ctx.coders = create_and_init_coders(config.numb_of_coders, ctx.dongles,
			&ctx);
	create_threads(&ctx);
	i = 0;
	while (i < ctx.config->numb_of_coders)
	{
		printf("Coder %i: %i compilaciones\n", ctx.coders[i].id,
			ctx.coders[i].compile_count);
		i++;
	}
	free_context(&ctx);
	return (0);
}
