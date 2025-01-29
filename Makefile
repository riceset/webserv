CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
NAME = a.out
SRC = $(shell find $(SRCDIR) -name "*.cpp") main.cpp
OBJ = $(SRC:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
SRCDIR = src
OBJDIR = objs
INCLUDEDIR = includes
INCLUDE = -I $(INCLUDEDIR)

all: $(NAME)

$(NAME): $(OBJ)
	@$(CXX) $(CXXFLAGS) $(OBJ) $(INCLUDE) -o $(NAME)
	@echo "Compilation done: $(NAME)"

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@
	@echo "Compiled: $< -> $@"

clean:
	@rm -rf $(OBJDIR)
	@echo "Object files deleted."

fclean: clean
	@rm -rf $(NAME)
	@echo "Executable deleted."

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

format:
	@echo "Formatting code..."
	@clang-format -i $(SRC) $(INCLUDEDIR)/*.hpp