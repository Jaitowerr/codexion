/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_executed.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aitorres <aitorres@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 00:00:00 by jaitowerr         #+#    #+#             */
/*   Updated: 2026/06/10 16:33:55 by aitorres         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders/codexion.h"


// void *coder_executed(void *arg)		//prueba de carlos melero para ver info sobre como coge la info los hilos
// {
// 	t_coder *self;

//     self = (t_coder *)arg;
// 	while (arg)
// 	{
// 		printf("%s\n","Holacracola");
// 		printf("%d\n", self->id);
// 	}
// 	return NULL;
// }

// void *coder_executed(void *arg)	//prueba de tiempo con print usleepp
// {
// 	t_coder *self;

// 	self = (t_coder *)arg;
// 	printf("%ld\n", get_current_time_ms());
// 		usleep(1100);
// 	printf("%ld\n", get_current_time_ms());
// 	return NULL;
// }





void	*coder_executed(void *arg)
{
	t_coder	*self;

	self = (t_coder *)arg;	//casteo porque recibe void, no sabe que es por lo tanto casteo y debe ir bien, no se si poner un if con la funcion free
	while (self->compile_count < self->config->number_of_compiles_required
		&& !self->ctx->someone_burned)
	{
		// printf("PROGRAMADOR ID-%i COMPILE COUNT: %i\n", self->id, self->compile_count);
		self->last_compile_ms = get_current_time_ms();	//guardo el tiempo porque empiezo a esperar ahora
		if (strcmp(self->config->scheduler, "fifo") == 0)
		{
			if (take_dongles_fifo(self))
				return (NULL);
		}
		else
			if (take_dongles_edf(self))
				return (NULL);
		// if (take_dongles(self))		//coger dongle
				// 	return (NULL);
		if (compile(self))			// compilar
		{
			release_and_cooldown(self); // liberar antes de salir si se quemó durante/tras compile
			return (NULL);
		}
		release_and_cooldown(self);	//cooldown
		if (self->compile_count >= self->config->number_of_compiles_required)
			break ; // o return (NULL); si quieres terminar el hilo directamente aquí
		if (do_debug(self))			//debugear ver codigo el programador
			return (NULL);
		if (do_refactor(self))		//Ccorregir errores de código
			return (NULL);
	}
	return (NULL);
}

