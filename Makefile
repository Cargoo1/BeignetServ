CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -I$(HEADER_DIR) -I$(HEADER_DIR)/$(EXC_DIR) -I$(HEADER_DIR)/$(PAR_DIR) -I$(HEADER_DIR)/$(CONF_DIR) -g3

NAME = WebServ



HEADER_DIR = includes
CONF_DIR = configClass
EXC_DIR = exception
PAR_DIR = parser
HEADER = $(HEADER_DIR)/$(EXC_DIR)/configException.hpp	\
		 $(HEADER_DIR)/$(PAR_DIR)/configParser.hpp		\
		 $(HEADER_DIR)/$(PAR_DIR)/Token.hpp

SRCS_DIR = source
CONF_DIR = configClass
EXC_DIR = exception
PAR_DIR = parser
SRCS = $(SRCS_DIR)/$(EXC_DIR)/configException.cpp	\
	   $(SRCS_DIR)/$(PAR_DIR)/configParser.cpp		\
	   $(SRCS_DIR)/$(PAR_DIR)/Token.cpp			\
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