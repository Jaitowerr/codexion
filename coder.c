/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aitorres <aitorres@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 00:00:00 by jaitowerr         #+#    #+#             */
/*   Updated: 2026/06/05 17:36:28 by aitorres         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders/codexion.h"

void init_coders(t_coder *coders, t_dongle *dongles, int count, t_config *config)
{
    int i;

    i = 0;
    while (i < count)
    {
        coders[i].id = i + 1;
        coders[i].compile_count = 0;
        coders[i].last_compile_start.tv_sec = 0;
        coders[i].last_compile_start.tv_usec = 0;
        coders[i].left_dongle = &dongles[i];
        coders[i].right_dongle = &dongles[(i + 1) % count];
        coders[i].config = config;
        i++;
    }
}

t_coder *create_and_init_coders(int count, t_dongle *dongles, t_config *config, t_context *ctx)
{
    t_coder *coders;

    coders = safe_malloc(sizeof(t_coder) * count, ctx);
    init_coders(coders, dongles, count, config);
    return (coders);
}

void *coder_routine(void *arg)
{
	t_coder *self;

    self = (t_coder *)arg;
    printf("Programador %d: ¡Listo!!! A quemar CPU!!\n", self->id);
    return NULL;
}

void *coder_executed(void *arg)
{
	t_coder *self;
	
	self = (t_coder *)arg;	//casteo porque recibe void, no sabe que es por lo tanto casteo y debe ir bien, no se si poner un if con la funcion free
	while(self->compile_count < self->config->number_of_compiles_required)
	{
		// printf("COMPILE COUNT: %i\n", self->compile_count);
		printf("PROGRAMADOR ID-%i COMPILE COUNT: %i\n", self->id, self->compile_count);
		
		//REFACTORIZAR
		usleep(self->config->time_to_refactor * 1000);		//lo cmabios de milisegundos a microsegundos
		printf(" - REFACTORIZANDO programador ID-%i\n", self->id);
		// printf("%d", usleep(self->config->time_to_refactor * 1000));
		
		//COGER DONGLES
		pthread_mutex_lock(&self->left_dongle->mutex);
		self->left_dongle->taken = true;	//luego eliminar y de la lista
		pthread_mutex_lock(&self->right_dongle->mutex);
		self->right_dongle->taken = true;
		printf("  - COGER DONGLES programador ID-%i\n", self->id);
		
		//COMPILAR
		printf("   - COMPILAR programador ID-%i\n", self->id);
		usleep(self->config->time_to_compile * 1000);
		
		//TERMINAR Y SOLTAR DOONGLES
		printf("    - TERMINAR Y SOLTAR DONGLE programador ID-%i\n", self->id);
		pthread_mutex_unlock(&self->left_dongle->mutex);
		self->left_dongle->taken = false;
		pthread_mutex_unlock(&self->right_dongle->mutex);
		self->right_dongle->taken = false;
		
		self->compile_count++;
	}
	return NULL;
}


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