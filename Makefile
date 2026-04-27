NAME = ircserv
OBJDIR = objects

FLAGS = -Wall -Wextra -Werror -std=c++98 
SOURCES = srcs/main.cpp \
			srcs/client/Client.cpp \
			srcs/client/ClientUtils.cpp \
			srcs/channel/Channel.cpp \
			srcs/channel/ChannelUtils.cpp \
			srcs/server/Commands.cpp \
			srcs/server/Parser.cpp \
			srcs/server/ServerRunning.cpp \
			srcs/server/SendTo.cpp \
			srcs/server/ServerUtils.cpp \
			srcs/server/Utils.cpp \
			srcs/server/Server.cpp

OBJECTS = $(SOURCES:srcs/%.cpp=$(OBJDIR)/%.o)

$(OBJDIR)/%.o: srcs/%.cpp
	@mkdir -p $(dir $@)
	c++ $(FLAGS) -c $< -o $@

all: $(NAME)

$(NAME): $(OBJECTS)
	c++ $(FLAGS) -o $(NAME) $(OBJECTS) -g

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

dev : fclean
	@echo "Adding files from current directory only..."
	@git add . && git diff --cached --quiet || (git commit -m "IRC - auto/dev" && git push) || echo "No changes to commit"

dev-force : fclean
	@echo "Adding files from current directory only..."
	@git add . && git diff --cached --quiet || (git commit -m "IRC - auto/dev force" && git push --force) || echo "No changes to commit"

.PHONY: all clean fclean re