CXX = c++
CXXFLAG = -Wall -Wextra -Werror -std=c++20
NAME = webserv 
SRCDIR = $(shell find . -type d  -not -name "includes" -not -path ".*" | xargs)
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

.PHONY: all clean fclean re
