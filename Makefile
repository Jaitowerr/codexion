NAME = codexion
CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread
SRC = main.c parser.c dongle.c coder.c utils.c thread.c timer.c coder_executed.c coder_routine.c coder_routine_two.c coder_routine_fifo.c coder_routine_fifo_two.c coder_routine_fifo_three.c coder_routine_edf.c coder_routine_edf_two.c coder_routine_edf_three.c monitor_routine.c
OBJ = $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJ)

run: all
	$(MAKE) clean

%.o: %.c
	$(CC) $(CFLAGS) -I./coders -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re

# valgrind ./codexion 3 500 200 100 100 2 100 fifo

# ./codexion 3 10000 1 1 1 100 1 fifo

# A. Prueba Básica (Sin quemarse, 3 coders)
# 	Objetivo: Ver que compilan y sueltan.
# 		Comando: ./codexion 3 800 200 200 200 3 50 fifo
# 	Qué observar: Deberías ver logs de has taken a dongle, is compiling, is debugging, is refactoring. Al final, el programa debe terminar limpiamente.

# B. Prueba de Burnout (Se queman)
# 	Objetivo: Ver que el monitor detecta que no compilan a tiempo.
# 		Comando: ./codexion 3 100 200 200 200 10 50 fifo
#	 Qué observar: El time_to_burnout es 100ms, pero compilar+debug+refactor tarda mucho más. Debería salir rápido el mensaje burned out (o tu mensaje actual de "DIE") y el programa debe parar.

# C. Prueba de Cooldown (Conflicto de recursos)
# 	Objetivo: Ver que respetan el tiempo de espera del dongle.
# 		Comando: ./codexion 2 1000 100 100 100 5 500 fifo
# 	Qué observar: Con dongle_cooldown de 500ms, verás pausas entre compilaciones. Si no hay deadlock, es buena señal.

# D. Prueba EDF (Para cuando lo implementemos)
# 	Comando: ./codexion 3 800 200 200 200 3 50 edf

# deadlock
# ./codexion 5 800 200 100 100 5 100 fifo
