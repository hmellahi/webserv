NAME = Server


SRC_SRV = main.cpp \
	  Socket/Socket.cpp



all : $(NAME)

$(NAME) :
	clang++ $(SRC_SRV) -o $(NAME)


clean:
	rm -rf $(NAME)


fclean: clean

re : fclean all

c: re
	./$(NAME)

.PHONY: all re clean fclean

