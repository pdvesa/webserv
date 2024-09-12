NAME	:= webserv
CC		:= c++
CFLAGS	:= -std=c++17 -Wall -Wextra -Werror

SRC_DIR	:= src

SRC		:= $(SRC_DIR)/webserv.cpp
OBJS	:= $(SRC:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re