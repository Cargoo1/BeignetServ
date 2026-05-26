CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -I$(HEADER_DIR)/$(EXEC_DIR) -I$(HEADER_DIR)/$(EXEP_DIR) -I$(HEADER_DIR)/$(PAR_DIR) -I$(HEADER_DIR)/$(CONF_DIR) -g3

NAME = WebServ



HEADER_DIR = includes
CONF_DIR = configClass
EXEP_DIR = exception
EXEC_DIR = execution
PAR_DIR = parser
HEADER = $(HEADER_DIR)/$(EXEP_DIR)/configException.hpp	\
		 $(HEADER_DIR)/$(PAR_DIR)/configParser.hpp		\
		 $(HEADER_DIR)/$(PAR_DIR)/Token.hpp				\
		 $(HEADER_DIR)/$(PAR_DIR)/parserUtils.hpp		\
		 $(HEADER_DIR)/$(CONF_DIR)/serverConfig.hpp		\
		 $(HEADER_DIR)/$(CONF_DIR)/locationConfig.hpp	\
		 $(HEADER_DIR)/$(EXEC_DIR)/Server.hpp			\
		 $(HEADER_DIR)/$(EXEC_DIR)/handle_request.hpp	\
		 $(HEADER_DIR)/$(EXEC_DIR)/parse_request.hpp	\
		 $(HEADER_DIR)/$(EXEC_DIR)/Request.hpp			\
		 $(HEADER_DIR)/$(EXEC_DIR)/run_server.hpp		\
		 $(HEADER_DIR)/$(EXEC_DIR)/Client.hpp			\
		 $(HEADER_DIR)/$(EXEC_DIR)/Header.hpp			\
		 $(HEADER_DIR)/$(EXEC_DIR)/HttpResponse.hpp		\
		 $(HEADER_DIR)/$(EXEC_DIR)/send_http_response.hpp	\

SRCS_DIR = source
CONF_DIR = configClass
EXEP_DIR = exception
PAR_DIR = parser
SRCS = $(SRCS_DIR)/$(EXEP_DIR)/configException.cpp	\
	   $(SRCS_DIR)/$(PAR_DIR)/configParser.cpp		\
	   $(SRCS_DIR)/$(PAR_DIR)/Token.cpp				\
	   $(SRCS_DIR)/$(PAR_DIR)/parserUtils.cpp		\
	   $(SRCS_DIR)/$(CONF_DIR)/serverConfig.cpp		\
	   $(SRCS_DIR)/$(CONF_DIR)/locationConfig.cpp	\
	   $(SRCS_DIR)/$(EXEC_DIR)/Server.cpp			\
	   $(SRCS_DIR)/$(EXEC_DIR)/handle_request.cpp	\
	   $(SRCS_DIR)/$(EXEC_DIR)/parse_request.cpp	\
	   $(SRCS_DIR)/$(EXEC_DIR)/Request.cpp			\
	   $(SRCS_DIR)/$(EXEC_DIR)/run_server.cpp		\
	   $(SRCS_DIR)/$(EXEC_DIR)/Client.cpp			\
	   $(SRCS_DIR)/$(EXEC_DIR)/Header.cpp			\
	   $(SRCS_DIR)/$(EXEC_DIR)/HttpResponse.cpp		\
	   $(SRCS_DIR)/$(EXEC_DIR)/send_http_response.cpp	\
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
