CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
NAME = config
SRC = $(shell find $(SRCDIR) -type f -name "*.cpp")
OBJ = $(SRC:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
SRCDIR = ..
OBJDIR = objs
INCLUDEDIR = ../../../includes/config/

all:
	@$(MAKE) init
	@$(MAKE) $(NAME)

init: cp
	$(eval SRC = $(shell find $(SRCDIR) -type f -name "*.cpp"))
	$(eval OBJ = $(SRC:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o))

cp :
	@echo "Copying main.txt ..."
	@cp main.txt ../main.cpp

$(NAME): $(OBJ)
	@echo "Linking: $(OBJ) -> $(NAME)"
	$(CXX) $(CXXFLAGS) $(OBJ) -I $(INCLUDEDIR) -o $(NAME)
	@echo "Compilation done: $(NAME)"
	@rm ../main.cpp

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@echo "Compiling: $< -> $@"
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) -I $(INCLUDEDIR) -c $< -o $@
	@echo "Compiled: $< -> $@"

clean:
	@rm -rf $(OBJDIR)
	@echo "Object files deleted."

fclean: clean
	@rm -rf $(NAME)
	@rm -rf ../main.cpp
	@echo "Executable deleted."

re: fclean all

PHONY: all clean fclean re