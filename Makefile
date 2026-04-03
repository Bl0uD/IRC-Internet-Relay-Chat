NAME = ircserv
OBJDIR = objects

FLAGS = -Wall -Wextra -Werror -std=c++98
SOURCES = srcs/main.cpp \
			srcs/client/Client.cpp \
			srcs/server/Server.cpp

OBJECTS = $(SOURCES:srcs/%.cpp=$(OBJDIR)/%.o)

$(OBJDIR)/%.o: srcs/%.cpp
	@mkdir -p $(dir $@)
	c++ $(FLAGS) -c $< -o $@

all: $(NAME)

$(NAME): $(OBJECTS)
	c++ $(FLAGS) -o $(NAME) $(OBJECTS)

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

dev : fclean
	@echo "Adding files from current directory only..."
	@git add . && git diff --cached --quiet || (git commit -m "IRC - auto/dev" && git push) || echo "No changes to commit"

.PHONY: all clean fclean re