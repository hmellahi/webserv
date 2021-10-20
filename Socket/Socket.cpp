#include "Socket.hpp"


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
	address.sin_port = htons(8080);
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	
	int addrlen = sizeof(address);

	if (bind(_socket_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
	{ 
		perror("bind failed"); 
		exit(1);
	}
	int cnListn;
	if ((cnListn = listen(_socket_fd, 1)) < 0)
	{
		perror("In listen");
		exit(1);
	}
	int cnAccept;
	while (1)
	{
		if ((cnAccept = accept(_socket_fd,(struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0)
		{
			perror("In accept");
			exit(1);
		}
		_cnAccept = cnAccept;
		char buffer[1024] = {0};

		//int readsocket = read( cnListn, buffer, 1024 );
		//std::cout <<  buffer << std::endl;
		
		write( _cnAccept, "Malaoui's WebServer\n", 20 );

		// if (readsocket < 0)
		// {
		// 	std::cout << "Nothing To read\n";
		// }
		//std::cout << 
		close(_cnAccept);
	}
}


Socket::~Socket()
{

}
