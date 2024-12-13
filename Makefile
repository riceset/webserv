CXX = c++
CXXFLAG = -Wall -Wextra -Werror -std=c++98
NAME = a.out
SRCDIR = $(shell find . -type d  -not -name "includes" -not -name "test" -not -path ".*" | xargs)
SRC = $(shell find . $(SRCDIR) -name "*.cpp" -type f | xargs)
OBJDIR = objs
OBJ = $(SRC:%.cpp=$(OBJDIR)/%.o)
INCLUDE = -I includes
COMPOSE_PATH = docker/docker-compose.yml


vpath	$(SRCDIR)

all: $(NAME)

$(NAME): $(OBJ)
	@$(CXX) $(CXXFLAG) $(OBJ) $(INCLUDE) $(LIBRARY) -o $(NAME)
	@echo "Compilation done"

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAG) $(INCLUDE) -c $< -o $@
	@echo "Compiled "$<" successfully"

clean:
	@rm -rf $(OBJDIR)
	@echo "Objs deleted"
	@echo "Clean done"

fclean: clean
	@rm -rf $(NAME)
	@echo "Fclean done"

re: fclean all

# docker command section

build:
	docker compose -f $(COMPOSE_PATH) build

run:
	docker compose -f $(COMPOSE_PATH) run cpp-env

down:
	docker compose -f $(COMPOSE_PATH) down

.PHONY: all clean fclean re
