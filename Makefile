NAME = ircserv
FILES = ft_irc.cpp
SRC_FILES = $(addprefix src/, $(FILES))
HEADERS = ft_irc.h
HEADER_FILES = $(addprefix include/, $(HEADERS))
OBJ_FILES = $(SRC_FILES:.cpp=.o)
CPPC = c++
GDB ?= -g
STD ?= -std=c++98
CPPFLAGS = -Wall -Wextra $(GDB) $(STD) # -Werror
INCLUDING = -I./include
RM = rm -rf

all: $(NAME)

$(NAME): $(SRC_FILES) $(HEADER_FILES)
	$(CPPC) $(CPPFLAGS) $(INCLUDING) $(SRC_FILES) -o $(NAME)

clean:
	$(RM) $(OBJ_FILES)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
