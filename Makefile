# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: hamza <hamza@student.42.fr>                +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/10/12 23:29:54 by hamza             #+#    #+#              #
#    Updated: 2021/12/18 23:21:35 by hamza            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

FLAGS	= --std=c++98 # -Wall -Wextra -Werror
CC		= c++
NAME	= webserv


ROOT		= main.cpp
REQUEST		= Request.cpp
RESPONSE	= Response.cpp
SOCKET		= Socket.cpp
CGI			= CGI.cpp
SERVER		= Server.cpp
UTILS		= MediaTypes/MediaTypes.cpp FileSystem/FileSystem.cpp Utils.cpp
PARSER		= Config/Config.cpp    ParseConfig/ParseConfig.cpp
INDEXING	= Indexing.cpp

DEBUG_FLAGS = -fsanitize=address -g

SRC	=	$(ROOT:%.cpp=./src/%.cpp)\
		$(REQUEST:%.cpp=./src/HTTP/Request/%.cpp)\
		$(PARSER:%.cpp=./src/Parser/%.cpp)\
		$(RESPONSE:%.cpp=./src/HTTP/Response/%.cpp)\
		$(UTILS:%.cpp=./src/utils/%.cpp)\
		$(SERVER:%.cpp=./src/Server/%.cpp)\
		$(SOCKET:%.cpp=./src/Socket/%.cpp)\
		$(CGI:%.cpp=./src/CGI/%.cpp)\
		$(INDEXING:%.cpp=./src/Indexing/%.cpp)

INCLUDE = -I src/Server -I src/HTTP/Request -I src/HTTP/Response  -I src/Parser -I src/Parser/ParseConfig -I  src/utils/MediaTypes -I src/utils/ -I src/CGI  -I src/Socket  -I src/Parser/Config -I src/Indexing

all: $(NAME)

$(NAME): $(SRC)
	$(CC) ${FLAGS} $(INCLUDE) $(SRC)  -o $(NAME)
	# rm -rf webserv*
	#./webserv 2>/dev/null

san : $(SRC)
	$(CC) ${FLAGS} ${DEBUG_FLAGS} $(INCLUDE) $(SRC)  -o $(NAME)

run : re
	./$(NAME)  2>/dev/null
clean:
	@rm -rf ${OBJ}

fclean: clean
	@rm -f $(NAME)

re: fclean all