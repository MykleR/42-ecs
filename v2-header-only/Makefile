TARGET 			:= tests

DIR_HEADERS		:= .
DIR_SOURCES		:= unittests
DIR_OBJECTS		:= .objs

SOURCES			:= $(wildcard $(DIR_SOURCES)/*.c)
OBJECTS			:= $(SOURCES:%.c=$(DIR_OBJECTS)/%.o)
DEPENDENCIES	:= $(OBJECTS:.o=.d)

CC				:= cc
CFLAGS			:= -Wall -Wextra -Werror -O3
IFLAGS			:= -I $(DIR_HEADERS)

DIR_DUP			= mkdir -p $(@D)

all: test

-include $(DEPENDENCIES)

test: $(TARGET)
	@(./$(TARGET)) && printf " $(MSG_PASSED)" || (printf " $(MSG_FAILED)") 
	@rm -f $(TARGET)
	@rm -rf $(DIR_OBJECTS)

$(TARGET): $(OBJECTS)
	@$(CC) $(CFLAGS) $(IFLAGS) $^ -o $@
	@printf " $(MSG_COMPILED)"

$(DIR_OBJECTS)/%.o: %.c
	@$(DIR_DUP)
	@$(CC) $(CFLAGS) $(IFLAGS) -c $< -o $@

.PHONY: all


RED			=	\033[31m
CYAN		=	\033[36m
BOLD		=	\033[1m
ITALIC		=	\033[3m
RESET		=	\033[0m
MSG_COMPILED	= $(CYAN)$(BOLD)$(ITALIC)■$(RESET)  compiled	$(BOLD)$@$(RESET) $(CYAN)successfully$(RESET)\n
MSG_FAILED	= \n$(RED)$(BOLD)$(ITALIC)■$(RESET) --- $(BOLD)❌ TESTS $(RED)FAILED ❌$(RESET) ---\n
MSG_PASSED	= \n$(CYAN)$(BOLD)$(ITALIC)■$(RESET) --- $(BOLD)✅ ALL TESTS $(CYAN)PASSED ✅$(RESET) ---\n
