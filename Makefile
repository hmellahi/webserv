NAME = Server


SRC_SRV = main.cpp \
	  Socket/Socket.cpp


SRC_CLI = client.cpp

all : $(NAME)

$(NAME) :
	clang++ $(SRC_SRV) -o $(NAME)
	clang++ $(SRC_CLI) -o client

clean:
	rm -rf $(NAME)
	rm -rf  client

fclean: clean

re : fclean all

c: re
	./$(NAME)

.PHONY: all re clean fclean

