# name of the executable
NAME = webserv

# variables
src_dir = ./src
obj_dir = ./obj

# compilator and compilation flags
CC = c++
CFLAGS = -Wall -Werror -Wextra --std=c++98 -g
INCLUDES = -I ./headers

# source files (recursively find all .cpp files)
SRCS = $(shell find $(src_dir) -name "*.cpp") main.cpp
OBJS = $(addprefix $(obj_dir)/,$(notdir $(SRCS:.cpp=.o)))

# Colors for output
GREEN = \033[0;32m
RED = \033[0;31m
NC = \033[0m # No Color

###RULES###
all: $(NAME)

$(NAME): $(OBJS)
	@echo "$(GREEN)Linking $(NAME)...$(NC)"
	@$(CC) $(CFLAGS) $(INCLUDES) $(OBJS) -o $@
	@echo "$(GREEN)Build successful!$(NC)"

# Create object files from source files (flat structure)
$(obj_dir)/%.o: $(src_dir)/%.cpp | $(obj_dir)
	@echo "$(GREEN)Compiling $<...$(NC)"
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Handle files in subdirectories - find the source file by pattern matching
$(obj_dir)/%.o: $(src_dir)/*/%.cpp | $(obj_dir)
	@echo "$(GREEN)Compiling $<...$(NC)"
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(obj_dir)/%.o: %.cpp | $(obj_dir)
	@echo "$(GREEN)Compiling $<...$(NC)"
	@$(CC) $(CFLAGS) -g $(INCLUDES) -c $< -o $@

# Create object directory
$(obj_dir):
	@mkdir -p $(obj_dir)

# Clean object files
clean:
	@echo "$(RED)Cleaning object files...$(NC)"
	@rm -rf $(obj_dir)

# Clean everything
fclean: clean
	@echo "$(RED)Cleaning executable...$(NC)"
	@rm -f $(NAME)

# Rebuild everything
re: fclean all

.PHONY: all clean fclean re debug
