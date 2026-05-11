SRCS = run_server.cpp parse_request.cpp Request.cpp handle_request.cpp
INCLUDES = ../includes/Request.hpp
C++ = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98 -g3
NAME = server
OBJS = ${SRCS:.cpp=.o}

all: $(NAME)

$(NAME): $(OBJS)
	$(C++) -o $(NAME) $(OBJS)

$(OBJS): $(SRCS) $(INCLUDES)
	$(C++) $(CFLAGS) -c $(SRCS)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all
