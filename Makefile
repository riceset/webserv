CXX = c++
CXXFLAG = -Wall -Wextra -Werror -std=c++98
NAME = a.out
SRCDIR = $(shell find . -type d  -not -name "includes" -not -name "test" -not -path ".*" | xargs)
SRC = $(shell find . $(SRCDIR) -name "*.cpp" -type f | xargs)
OBJDIR = objs
OBJ = $(SRC:%.cpp=$(OBJDIR)/%.o)
INCLUDE = -I includes


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

# Paths
COMPOSE_PATH = docker/docker-compose.yml

# Commands
build:
	@echo "Building Docker image..."
	docker compose -f $(COMPOSE_PATH) build

up:
	@echo "Starting Docker container..."
	docker compose -f $(COMPOSE_PATH) up -d

down:
	@echo "Stopping and removing Docker container..."
	docker compose -f $(COMPOSE_PATH) down

rebuild: down build up

logs:
	@echo "Displaying container logs..."
	docker compose -f $(COMPOSE_PATH) logs -f

exec:
	@echo "Accessing the running container..."
	docker compose -f $(COMPOSE_PATH) exec cpp-env /bin/bash

cleanup:
	@echo "Removing unused Docker volumes and system resources..."
	docker system prune -f
	docker volume prune -f

remove-images:
	@echo "Removing all unused Docker images..."
	docker image prune -a -f

.PHONY: all clean fclean re
