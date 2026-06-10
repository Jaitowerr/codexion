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
		printf("PROGRAMADOR ID-%i COMPILE COUNT: %i\n", self->id, self->compile_count);
		self->last_compile_ms = get_current_time_ms();	//guardo el tiempo porque empiezo a esperar ahora
		if (take_dongles(self))		//coger dongle
			return (NULL);
		if (compile(self))			// compilar
			return (NULL);
		release_and_cooldown(self);	//cooldown
		if (do_debug(self))			//debugear ver codigo el programador
			return (NULL);
		if (do_refactor(self))		//Ccorregir errores de código
			return (NULL);
	}
	return (NULL);
}




// void *coder_executed(void *arg)
// {
// 	t_coder *self;
// 	long	now_ms;
// 	long	next_available;
	
// 	self = (t_coder *)arg;	

// 	while(self->compile_count < self->config->number_of_compiles_required && !self->ctx->someone_burned)
// 	{
// 		printf("PROGRAMADOR ID-%i COMPILE COUNT: %i\n", self->id, self->compile_count);
		
// 		self->last_compile_ms = get_current_time_ms();	//guardo el tiempoo porque empiezo a esperar ahora

// 		while ((get_current_time_ms() < self->left_dongle->available_at_ms) || (get_current_time_ms() < self->right_dongle->available_at_ms))
// 		{
// 			if (check_burnout(self))
// 				return (NULL);

// 			usleep(1000); 	//Mientras izq o dr no esté disponible, duerme el hilo o espera 0.1 milisegundo, mejor prueba, mas consumo de cpu
// 		}

// 		if (check_burnout(self))
// 			return (NULL);


// 		//COGER DONGLES
// 		pthread_mutex_lock(&self->left_dongle->mutex);
// 		pthread_mutex_lock(&self->right_dongle->mutex);
// 															// if dongle is disppoonible
// 		self->left_dongle->taken = true;					//luego eliminar y de la lista
// 		self->right_dongle->taken = true;
// 		printf("  - COGER DONGLES programador ID-%i izq %i dr %i\n", self->id, self->left_dongle->id, self->right_dongle->id);



// 		//COMPILAR
// 		printf("   - COMPILAR programador ID-%i\n", self->id);
// 		usleep(self->config->time_to_compile * 1000);


// 		now_ms = get_current_time_ms();
// 		next_available = now_ms + self->config->dongle_cooldown;

// 		//COOLDOWN - TERMINAR Y SOLTAR DOONGLES
// 		self->left_dongle->available_at_ms = next_available; //el cooldown le amrcamos cuando estará disponible d enuevo
// 		self->left_dongle->taken = false;
// 		pthread_mutex_unlock(&self->left_dongle->mutex);

// 		self->right_dongle->available_at_ms = next_available; //el cooldown le amrcamos cuando estará disponible d enuevo
// 		self->right_dongle->taken = false;
// 		pthread_mutex_unlock(&self->right_dongle->mutex);

		
// 		printf("    - TERMINAR Y SOLTAR DONGLE programador ID-%i\n", self->id);

// 		self->compile_count++;

// 		printf(" - DEBUGGING programador ID-%i\n", self->id);
// 		usleep(self->config->time_to_debug * 1000);

// 		//REFACTORIZAR
// 		printf(" - REFACTORIZANDO programador ID-%i\n", self->id);
// 		usleep(self->config->time_to_refactor * 1000);
// 	}
// 	return NULL;
// }


// void *coder_executed(void *arg)
// {
//     t_coder *self;

//     self = (t_coder *)arg;
    
//     // Mientras no haya llegado al número de compilaciones necesarias...
//     while (self->compile_count < self->config->number_of_compiles_required)
//     {
//         // 1. Pensar/Refactorizar (Tardar un tiempo)
//         printf("Programador %d: Estoy pensando/refactorizando...\n", self->id);
//         usleep(self->config->time_to_refactor * 1000); // usleep usa microsegundos, multiplicamos por 1000 para milisegundos

//         // 2. Intentar coger dongles (Aquí es donde usaremos Mutex más adelante)
//         printf("Programador %d: Intentando coger dongles...\n", self->id);
        
//         // 3. Compilar
//         printf("Programador %d: Comienza la compilación %d...\n", self->id, self->compile_count + 1);
//         usleep(self->config->time_to_compile * 1000);
        
//         // 4. Terminar compilación y soltar dongles
//         self->compile_count++;
//         printf("Programador %d: ¡Compilación %d terminada! Suelto dongles.\n", self->id, self->compile_count);
//     }
    
//     printf("Programador %d: He terminado todas mis tareas. ¡Me voy a casa!\n", self->id);
//     return (NULL);
// }



// void create(t_coder  *coders)
// {
//     coders = malloc(sizeof(t_coder) * config.number_of_coders)
//     if (!coders)
//     {
//         printf("Error: No se pudo reservar memoria.\n");
//         return 1;
//     }
// }