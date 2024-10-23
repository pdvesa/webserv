# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: dbarrene <dbarrene@student.hive.fi>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/09/01 18:14:56 by dbarrene          #+#    #+#              #
#    Updated: 2024/10/23 15:59:58 by dbarrene         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME  = webserv
SRCDIR = src
OBJDIR = obj
INCDIR = includes
CC = c++
CFLAGS = -Wall -Wextra -Werror -pedantic -g -fsanitize=address
SRCS = $(SRCDIR)/main.cpp \
		$(SRCDIR)/Controller.cpp \
		$(SRCDIR)/Socket.cpp \
		$(SRCDIR)/Client.cpp \
		$(SRCDIR)/HttpRequest.cpp \
		$(SRCDIR)/HttpResponse.cpp \
		$(SRCDIR)/BodyChunk.cpp \

OBJS =  $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRCS))

all: $(NAME)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@$(CC) $(CFLAGS) -I $(INCDIR) -c $< -o $@

$(NAME): $(OBJDIR) $(OBJS)
	@$(CC) $(CFLAGS) $(OBJS) -o $(NAME) -I $(INCDIR)
	@echo "Compiling $(NAME)"
	@echo "Compilation finished :)"

$(OBJDIR):
	@mkdir -p $(OBJDIR)

clean:
	@echo "Removing objects"
	@rm -f $(OBJS)
	@rm -rf $(OBJDIR)
fclean: clean
	@rm -f $(NAME)
	@echo "Removing $(NAME)"

re: fclean all
