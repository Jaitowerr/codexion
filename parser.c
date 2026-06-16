/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aitorres <aitorres@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 00:00:00 by jaitowerr         #+#    #+#             */
/*   Updated: 2026/06/05 15:57:24 by aitorres         ###   ########.fr       */
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

static bool	parse_int_arg(long *value, char *arg, const char *message)
{
	if (!is_number(arg) || atoi(arg) <= 0)
	{
		printf("%s\n", message);
		return (false);
	}
	*value = atoi(arg);
	return (true);
}

static bool	validate_coders(char *arg)
{
	if (!is_number(arg) || atoi(arg) < 1)
	{
		printf("Error: number_of_coders debe ser un número, mayor de 0 y menor a 2147483647.\n");
		return (false);
	}
	return (true);
}

static bool	parse_int_params(t_config *config, char **argv)
{
	if (!parse_int_arg(&config->time_to_burnout, argv[2],
			"Error: time_to_burnout debe ser un número, mayor de 0 y menor a 2147483647."))
		return (false);
	if (!parse_int_arg(&config->time_to_compile, argv[3],
			"Error: time_to_compile debe ser un número, mayor de 0 y menor a 2147483647."))
		return (false);
	if (!parse_int_arg(&config->time_to_debug, argv[4],
			"Error: time_to_debug debe ser un número, mayor de 0 y menor a 2147483647."))
		return (false);
	if (!parse_int_arg(&config->time_to_refactor, argv[5],
			"Error: time_to_refactor debe ser un número, mayor de 0 y menor a 2147483647."))
		return (false);
	if (!parse_int_arg(&config->number_of_compiles_required,
			argv[6],
			"Error: number_of_compiles_required debe ser un número, mayor de 0 y menor a 2147483647."))
		return (false);
	if (!parse_int_arg(&config->dongle_cooldown, argv[7],
			"Error: dongle_cooldown debe ser un número, mayor de 0 y menor a 2147483647."))
		return (false);
	return (true);
}

bool	parse_arguments(int argc, char **argv, t_config *config)
{
	if (argc != 9)
	{
		printf("Uso: %s number_of_coders\n time_to_burnout\n",
			argv[0]);
		printf("      time_to_compile time_to_debug\n"
			"      time_to_refactor number_of_compiles_required\n"
			"      dongle_cooldown"
			"      scheduler\n");
		return (false);
	}
	if (!validate_coders(argv[1]))
		return (false);
	config->number_of_coders = atoi(argv[1]);
	if (!parse_int_params(config, argv))
		return (false);
	if (strcmp(argv[8], "fifo") != 0 && strcmp(argv[8], "edf") != 0)
	{
		printf("Error: scheduler debe ser 'fifo' o 'edf'.\n");
		return (false);
	}
	config->scheduler = argv[8];
	return (true);
}
