NAME = codexion
CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread
SRC = main.c parser.c dongle.c coder.c utils.c thread.c timer.c coder_executed.c coder_routine.c coder_routine_two.c monitor_routine.c
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