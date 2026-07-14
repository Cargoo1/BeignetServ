CC = c++
CFLAGS = -Werror -Wextra -Wall -std=c++98
FILES = megaphone.cpp
OBJECTS = $(FILES:.cpp=.o)
NAME = megaphone

all: $(NAME)

$(NAME): $(OBJECTS)
	$(CC) -o $(NAME) $(OBJECTS)

$(OBJECTS): $(FILES)
	$(CC) -c $(CFLAGS) $<

clean:
	@rm -f $(OBJECTS)
	@echo "binaries cleaned"

fclean: clean
	@rm -f $(NAME)
	@echo "executable cleaned"

re: fclean all
