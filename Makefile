# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: hamza <hamza@student.42.fr>                +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/10/12 23:29:54 by hamza             #+#    #+#              #
#    Updated: 2021/10/20 23:06:16 by hamza            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

FLAGS	=# -Wall -Wextra -Werror
CC		= clang++
NAME	= webserv


ROOT		= main.cpp
REQUEST		= Request.cpp
RESPONSE	= Response.cpp
SOCKET		= Socket.cpp
CGI			= CGI.cpp
SERVER		= Server.cpp
UTILS		= MediaTypes/MediaTypes.cpp FileSystem/FileSystem.cpp Utils.cpp
PARSER		= Config/Config.cpp    ParseConfig/ParseConfig.cpp

DEBUG_FLAGS = -fsanitize=address -g

SRC	=	$(ROOT:%.cpp=./src/%.cpp)\
		$(REQUEST:%.cpp=./src/HTTP/Request/%.cpp)\
		$(PARSER:%.cpp=./src/Parser/%.cpp)\
		$(RESPONSE:%.cpp=./src/HTTP/Response/%.cpp)\
		$(UTILS:%.cpp=./src/utils/%.cpp)\
		$(SERVER:%.cpp=./src/Server/%.cpp)\
		$(SOCKET:%.cpp=./src/Socket/%.cpp)\
		$(CGI:%.cpp=./src/CGI/%.cpp)

INCLUDE = -I src/Server -I src/HTTP/Request -I src/HTTP/Response  -I src/Parser -I src/Parser/ParseConfig -I  src/utils/MediaTypes -I src/utils/ -I src/CGI  -I src/Socket  -I src/Parser/Config

all: $(NAME)

$(NAME): $(SRC)
	$(CC) ${FLAGS} $(INCLUDE) $(SRC)  -o $(NAME)
	# rm -rf webserv*

san : $(SRC)
	$(CC) ${FLAGS} ${DEBUG_FLAGS} $(INCLUDE) $(SRC)  -o $(NAME)

clean:
	@rm -rf ${OBJ}

fclean: clean
	@rm -f $(NAME)

re: fclean all