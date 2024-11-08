# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: dbarrene <dbarrene@student.hive.fi>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/09/01 18:14:56 by dbarrene          #+#    #+#              #
#    Updated: 2024/09/12 17:11:30 by dbarrene         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME  = webserv
SRCDIR = src
OBJDIR = obj
INCDIR = includes
CC = c++
CFLAGS = -Wall -Wextra -Werror -pedantic -g

SRCS = $(SRCDIR)/run/main.cpp \
       $(SRCDIR)/controller/WebservController.cpp \
       $(SRCDIR)/model/Socket.cpp \
       $(SRCDIR)/model/Client.cpp \
       $(SRCDIR)/util/CppSplit.cpp \
       $(SRCDIR)/util/IsBlank.cpp \
       $(SRCDIR)/util/Parsing.cpp \
       $(SRCDIR)/util/SpacesClean.cpp \
       $(SRCDIR)/util/StrictUtoi.cpp \
	   $(SRCDIR)/util/EpollUtils.cpp \
	   $(SRCDIR)/model/RouteConfig.cpp \
       $(SRCDIR)/model/ServerConfig.cpp \
	   $(SRCDIR)/model/Server.cpp \

OBJS = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRCS))

all: $(NAME)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)  # Create the directory for the object file if it doesn't exist
	@$(CC) $(CFLAGS) -I $(INCDIR) -c $< -o $@

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) $(OBJS) -o $(NAME) -I $(INCDIR)
	@echo "Compiling $(NAME)"
	@echo "Compilation finished :)"

clean:
	@echo "Removing objects"
	@rm -f $(OBJS)
	@rm -rf $(OBJDIR)

fclean: clean
	@rm -f $(NAME)
	@echo "Removing $(NAME)"

re: fclean all

