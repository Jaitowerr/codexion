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

#include "coders/codexion.h"

static bool	is_number(char *str)
{
	if (!str || *str == '\0')
		return (false);
	while (*str)
	{
		if (*str < '0' || *str > '9')
			return (false);
		str++;
	}
	return (true);
}

bool	parse_arguments(int argc, char **argv, t_config *config)
{
	if (argc != 9)
	{
		printf("Uso: %s number_of_coders time_to_burnout time_to_compile\n",
			argv[0]);
		printf("      time_to_debug time_to_refactor number_of_compiles_required\n"
			"      dongle_cooldown scheduler\n");
		return (false);
	}
	if (!is_number(argv[1]) || atoi(argv[1]) < 2)
	{
		printf("Error: number_of_coders debe ser mayor a 1.\n");
		return (false);
	}
	config->number_of_coders = atoi(argv[1]);
	if (!is_number(argv[2]) || atoi(argv[2]) <= 0)
	{
		printf("Error: time_to_burnout debe ser un número mayor de 0.\n");
		return (false);
	}
	config->time_to_burnout = atoi(argv[2]);
	if (!is_number(argv[3]) || atoi(argv[3]) <= 0)
	{
		printf("Error: time_to_compile debe ser un entero mayor de 0.\n");
		return (false);
	}
	config->time_to_compile = atoi(argv[3]);
	if (!is_number(argv[4]) || atoi(argv[4]) <= 0)
	{
		printf("Error: time_to_debug debe ser un entero mayor de 0.\n");
		return (false);
	}
	config->time_to_debug = atoi(argv[4]);
	if (!is_number(argv[5]) || atoi(argv[5]) <= 0)
	{
		printf("Error: time_to_refactor debe ser un entero mayor de 0.\n");
		return (false);
	}
	config->time_to_refactor = atoi(argv[5]);
	if (!is_number(argv[6]) || atoi(argv[6]) <= 0)
	{
		printf("Error: number_of_compiles_required debe ser un entero mayor de 0.\n");
		return (false);
	}
	config->number_of_compiles_required = atoi(argv[6]);
	if (!is_number(argv[7]) || atoi(argv[7]) <= 0)
	{
		printf("Error: dongle_cooldown debe ser un entero mayor de 0.\n");
		return (false);
	}
	config->dongle_cooldown = atoi(argv[7]);
	if (strcmp(argv[8], "fifo") != 0 && strcmp(argv[8], "edf") != 0)
	{
		printf("Error: scheduler debe ser 'fifo' o 'edf'.\n");
		return (false);
	}
	config->scheduler = argv[8];
	return (true);
}
