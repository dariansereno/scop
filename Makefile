.DEFAULT_GOAL = all

NAME = scop

CXX ?= g++
CXXFLAGS = -std=c++2b -g3 -o2 #-Wall -Wextra -Werror 

LDFLAGS =  -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr

DIR_SRC = srcs
DIR_OBJ = .obj

SRC := $(shell find $(DIR_SRC) -name "*.cpp")
INC := $(shell find $(DIR_SRC) -name "*.hpp")
OBJ := $(patsubst $(DIR_SRC)/%,$(DIR_OBJ)/%,$(SRC:.cpp=.o))
INCFLAGS = $(addprefix -I./, $(shell find $(DIR_SRC) -type d))

$(DIR_OBJ):
	@mkdir -p $(DIR_OBJ)

$(DIR_OBJ)/%.o : $(DIR_SRC)/%.cpp $(INC)
	@mkdir -p $(dir $@)
	@printf "\033[0;33mGenerating scop object: %-38.38s \r" $@
	@$(CXX) $(CXXFLAGS) $(INCFLAGS) -o $@ -c $< 

$(NAME): $(OBJ)
	@$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME) $(LDFLAGS)
	@echo "\033[1;32m\nScop: Done!\033[0m"

all: $(DIR_OBJ) $(NAME)

clean:
	@rm -f $(OBJ)

fclean: clean
	@rm -f $(NAME)
	@rm -rf $(DIR_OBJ)

re: fclean
	@$(MAKE) all

.PHONY: all clean fclean re