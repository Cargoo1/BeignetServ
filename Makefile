CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -I$(HEADER_DIR) -g3

NAME = WebServ

HEADER_DIR = incldue
HEADER = $(HEADER_DIR)/WebServ.hpp

SRCS_DIR = source
SRCS = $(SRCS_DIR)/WebServ.cpp \
	   $(SRCS_DIR)/main.cpp

OBJ_DIR = obj
OBJS = $(SRCS:$(SRCS_DIR)/%.cpp=$(OBJ_DIR)/%.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRCS_DIR)/%.cpp $(HEADER)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re