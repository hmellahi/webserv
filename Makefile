NAME = webserv

SRC  = 	main.cpp				\
		parser/ParseConfig.cpp		\
		parser/Config.cpp			\
		parser/Request.cpp			\
		Indexing/Indexing.cpp		\
		utils/Utils.cpp				\

FLAGS = -std=c++98 #-Wall -Wextra -Werror 

all : $(NAME)

$(NAME) : ${SRC}
	c++  -o $(NAME) ${SRC} ${FLAGS}

clean :
	/bin/rm -f $(OBJ)

fclean : clean
	/bin/rm -f $(NAME)

re : fclean all