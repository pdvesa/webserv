NAME  = webserv
SRCDIR = src
OBJDIR = obj
INCDIR = includes
CC = c++
CFLAGS = -Wall -Wextra -Werror -pedantic -g -std=c++20

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
       $(SRCDIR)/model/HttpRequest.cpp \
       $(SRCDIR)/model/HttpResponse.cpp \
	   $(SRCDIR)/model/CGIHandler.cpp \
	   $(SRCDIR)/util/VecBuffCmp.cpp \
	   $(SRCDIR)/model/MultipartFormDataBody.cpp \
	   $(SRCDIR)/model/RequestBody.cpp

OBJS = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRCS))

all: $(NAME)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)  # Create the directory for the object file if it doesn't exist
	@$(CC) $(CFLAGS) -I $(INCDIR) -c $< -o $@

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) $(OBJS) -o $(NAME) -I $(INCDIR)
	@echo "Compiling $(NAME)"
	@echo "Compilation finished :)"



debug:
	make
	valgrind --leak-check=full ./$(NAME) configs/sampleconf

test:
	make
	./$(NAME) configs/sampleconf


clean:
	@echo "Removing objects"
	@rm -f $(OBJS)
	@rm -rf $(OBJDIR)

fclean: clean
	@rm -f $(NAME)
	@echo "Removing $(NAME)"

re: fclean all

.PHONY: test