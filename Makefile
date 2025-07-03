NAME 			:= 42-ecs.a

DIR_HEADERS		:= headers
DIR_SOURCES		:= sources
DIR_OBJS		:= .objs

include $(DIR_SOURCES)/sources.mk

OBJS			:= $(addprefix $(DIR_OBJS)/, $(SOURCES:%.c=%.o))

CC				:= cc
CFLAGS			:= -Wall -Wextra -Werror -O3
IFLAGS			:= -I $(DIR_HEADERS)

GREEN			= \033[0;32m
BOLD			= \033[1m
END				= \033[0m
DIR_DUP			= mkdir -p $(@D)

all: $(NAME)

$(NAME): $(OBJS)
	@ar -rcs $@ $^
	@printf "$(BOLD)$(NAME)$(END) compiled $(GREEN)successfully$(END)\n"

$(DIR_OBJS)/%.o: $(DIR_SOURCES)/%.c
	@$(DIR_DUP)
	@$(CC) $(CFLAGS) $(IFLAGS) -c $< -o $@

clean:
	@rm -rf $(DIR_OBJS)
	@printf "Cleaned $(BOLD)$(DIR_OBJS)$(END) of $(NAME)\n"

fclean: clean
	@rm -f $(NAME)
	@printf "Cleaned $(BOLD)$(NAME)$(END)\n"

re: fclean all

.PHONY: clean fclean re all
