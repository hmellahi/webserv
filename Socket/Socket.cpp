#include "Socket.hpp"

#include<fstream>
#include<sstream>


std::string Socket::handle_cgi( void ) {

    int filedes[2];
    char * a[] ={"php", "index.html", (char*)0};
    std::string f = "";

    if (pipe(filedes) == -1) {
        perror("pipe");
        exit(1);
    }

    pid_t pid = fork();
        
    if (pid == -1) {
        perror("fork");
        exit(1);
    }

    if (pid > 0) {
        close(filedes[1]);
        FILE *result;

        result = fdopen(filedes[0], "r");
        char c;
        while((c = fgetc(result)) != EOF)
            f += c;
        close(filedes[0]);

        fclose(result);

    }
    else     if (pid == 0)
    {
        dup2(filedes[1], 1);
        close(filedes[1]);
        close(filedes[0]);

        execve("/usr/bin/php", a, NULL);
        perror("execve");
        exit(1);
    }
    return f;
}

std::string	handle_requests( void )
{
	std::ifstream f("index.html"); //taking file as inputstream
	std::string str;
	
	if(f) {
		std::ostringstream ss;
		ss << f.rdbuf(); // reading data
		str = ss.str();
	}
	std::cout << str << std::endl;
	return str;
}

Socket::Socket( int protocol )
{
	// create new socket inside kernel and returns an integer which used as socket descriptor.
	// simply creating
	if ((_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("cannot create socket");
		exit(1);
	}
	
	// binding == assigning a transport address to the socket
	struct sockaddr_in address;
	
	
	/* htonl converts a long integer (e.g. address) to a network representation */ 
	/* htons converts a short integer (e.g. port) to a network representation */ 
	
	
	address.sin_family = AF_INET;
	address.sin_port = htons(protocol);
	address.sin_addr.s_addr = INADDR_ANY;
	memset(address.sin_zero, '\0', sizeof address.sin_zero);
	int addrlen = sizeof(address);

	if (bind(_socket_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
	{ 
		perror("bind failed"); 
		exit(1);
	}

	if (listen(_socket_fd, 1) < 0)
	{
		perror("In listen");
		exit(1);
	}
	char *hello = "HTTP/1.1 200 OK\nContent-Type: text/html\n\nHello world!";
	int cnAccept;

	std::string index_html;
	std::string res;


	while (1)
	{
		if ((cnAccept = accept(_socket_fd,(struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0)
		{
			perror("In accept");
			exit(1);
		}
		char buffer[1024] = {0};

		int readsocket = read( cnAccept, buffer, 1024 );
		std::cout <<  buffer << std::endl;
		
		printf("%s\n",buffer );
        //write( cnAccept, hello, strlen(hello) );
        //printf("------------------Hello message sent-------------------\n");
		//index_html = handle_requests();
		index_html = handle_cgi();
		res = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: ";
		// Content lenght represents only in this case index.html length
		
		res += std::to_string( index_html.length() ) + "\r\n\r\n";
		
		res += index_html;
		std::cout << res << std::endl;
	
		write( cnAccept, res.c_str(), res.length() + 1);
		//index_html = handle_requests();
		//send( cnAccept, res.c_str(), res.length() + 1, 0);
		if (readsocket < 0)
		{
			std::cout << "Nothing To read\n";
		}
		close(cnAccept);
		//exit(0);
	}
}


Socket::~Socket()
{

}
