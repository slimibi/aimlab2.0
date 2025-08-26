NAME = aim_trainer

CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c99

SRCDIR = src
OBJDIR = obj
INCDIR = include

SOURCES = main.c game.c duck.c utils.c menu.c
OBJECTS = $(SOURCES:%.c=$(OBJDIR)/%.o)

LIBS = -lcsfml-graphics -lcsfml-audio -lcsfml-system -lm

# Colors for output
GREEN = \033[0;32m
YELLOW = \033[0;33m
RED = \033[0;31m
NC = \033[0m # No Color

all: $(NAME)

$(NAME): $(OBJECTS)
	@echo "$(GREEN)Linking $(NAME)...$(NC)"
	@$(CC) $(OBJECTS) -o $(NAME) $(LIBS)
	@echo "$(GREEN)Build successful!$(NC)"

$(OBJDIR)/%.o: %.c duck_hunt.h
	@mkdir -p $(OBJDIR)
	@echo "$(YELLOW)Compiling $<...$(NC)"
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "$(RED)Cleaning object files...$(NC)"
	@rm -rf $(OBJDIR)

fclean: clean
	@echo "$(RED)Cleaning executable...$(NC)"
	@rm -f $(NAME)

re: fclean all

install_deps:
	@echo "$(GREEN)Installing CSFML dependencies...$(NC)"
	@if command -v apt-get > /dev/null; then \
		sudo apt-get update && sudo apt-get install -y libcsfml-dev; \
	elif command -v pacman > /dev/null; then \
		sudo pacman -S csfml; \
	elif command -v brew > /dev/null; then \
		brew install csfml; \
	elif command -v dnf > /dev/null; then \
		sudo dnf install CSFML-devel; \
	else \
		echo "$(RED)Package manager not supported. Please install CSFML manually.$(NC)"; \
	fi

run: $(NAME)
	@echo "$(GREEN)Running Duck Hunt...$(NC)"
	@./$(NAME)

debug: CFLAGS += -g -DDEBUG
debug: $(NAME)

.PHONY: all clean fclean re install_deps run debug