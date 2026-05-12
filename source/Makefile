SRCS = run_server.cpp parse_request.cpp Request.cpp handle_request.cpp Client.cpp
INCLUDES = ../includes/Request.hpp \
		   ../includes/Client.hpp
C++ = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98 -g3 -fsanitize=address
NAME = server
OBJS = ${SRCS:.cpp=.o}

all: $(NAME)

$(NAME): $(OBJS)
	$(C++) $(CFLAGS) -o $(NAME) $(OBJS)

$(OBJS): $(SRCS) $(INCLUDES)
	$(C++) $(CFLAGS) -c $(SRCS)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all
