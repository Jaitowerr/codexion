# Codexion
compite contra el tiempo en este emocionante desafío de concurrencia. Orquesta a múltiples personas que programan compitiendo por un número limitado de dongles USB mediante hilos POSIX, mutexes y planificación inteligente—domina la sincronización de recursos antes de que llegue el agotamiento.


cuando compilemos, que va a recibir el programa:
El programa debe recibir los siguientes argumentos (todos obligatorios):
    number_of_coders 
    time_to_burnout
    time_to_compile 
    time_to_debug 
    time_to_refactor 
    number_of_compiles_required 
    dongle_cooldown 
    scheduler

    Ej: ./codexion 4 500 200 200 200 2 100 fifo
8 argumentos mas el nombre dle rpograma 9 argumentos
Necesitamos un parseo que compruebe:
    - argc == 9
    - minimo 2 programadores number_of_coders sea ≥ 1
    - que los datos sean numericos excepto scheduler, y todos ellos mayores a 0
    - scheduler tiene que ser fifo o edf - compararemos con strcpm()


Necesitamos un mak con:
    - make        # Compila
    - make clean  # Borra objetos
    - make fclean # Borra ejecutable
    - make re     # Limpia y reconstruye