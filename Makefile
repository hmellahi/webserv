# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: hamza <hamza@student.42.fr>                +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/10/12 23:29:54 by hamza             #+#    #+#              #
#    Updated: 2021/10/16 00:17:01 by hamza            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

FLAGS	=# -Wall -Wextra -Werror
CC		= clang++
NAME = server


ROOT		= main.cpp
REQUEST		= 
RESPONSE	= 
CGI			= 
SERVER		= #Server.hpp
UTILS		= 

SRC	=	$(ROOT:%.cpp=./src/%.cpp)\
		$(REQUEST:%.cpp=./src/Request/%.cpp)\
		$(UTILS:%.cpp=./src/utils/%.cpp)\
		$(RESPONSE:%.cpp=./src/Response/%.cpp)\
		$(SERVER:%.cpp=./src/Server/%.cpp)\
		$(CGI:%.cpp=./src/CGI/%.cpp)

INCLUDE = -I src/Server -I src/Request -I src/Response -I src/Config

all: $(NAME)

$(NAME): $(SRC)
	$(CC) ${FLAGS} $(INCLUDE) $(SRC)  -o $(NAME)

san : $(SRC)
	$(CC) ${FLAGS}  $(SRC)  -o $(NAME)

clean:
	@rm -rf ${OBJ}

fclean: clean
	@rm -f $(NAME)

re: fclean all