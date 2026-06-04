NAME = codexion
CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread
SRC = main.c parser.c
OBJ = $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -I./coders -o $(NAME) $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -I./coders -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re