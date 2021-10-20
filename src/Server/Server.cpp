#include "Server.hpp"

void	Server::addClients(std::vector<Socket> clients, int &max_fd, fd_set &readfds)
{
	int	fd;
	//add child sockets to set
	for (int i = 0 ; i < clients.size() ; i++)
	{
		//socket descriptor
		fd = clients[i];
			
		//if valid socket descriptor then add to read list
		if(fd > 0)
			FD_SET( fd , &readfds);
			
		//highest file descriptor number, need it for the select function
		if(fd > max_fd)
			max_fd = fd;
	}
}

void	Server::waitingForConnections(int &activity, fd_set &readfds)
{
	activity = select(FD_SETSIZE , &readfds , NULL , NULL , NULL);
	if ((activity < 0) && (errno!=EINTR))
	{
		printf("select error");
	}
}

void	Server::addServers(std::vector<Socket> &sockets, int &max_sd, fd_set &readfds)
{
	for (int i = 0; i < sockets.size(); i++)
		FD_SET(sockets[i], &readfds);
	// reset max_sd
	max_sd = sockets[sockets.size() - 1];
}

void	Server::acceptNewConnection(std::vector<Socket> &clients, std::vector<Socket> &serversSockets, struct sockaddr_in &address, int &addrlen, fd_set &readfds)
{
	int socketDescriptor;
	Socket new_socket;

	for (int i = 0; i < serversSockets.size(); i++)
	{
		socketDescriptor = serversSockets[i];
		if (FD_ISSET(socketDescriptor, &readfds))
		{
			new_socket = Socket::acceptConnection(socketDescriptor, address, addrlen);
			// if a position is empty
			if (clients.size() < FD_SETSIZE)
				clients.push_back(new_socket);
		}
	}
}

void	Server::RecvAndSend(std::vector<Socket> &clients, fd_set &readfds,  std::vector<Server> &servers)
{
	int sd;
	char requestBody[1025];
	int valread;

	for (int i = 0; i < clients.size(); i++)
	{
		sd = clients[i];	
		if (FD_ISSET( sd , &readfds))
		{
			//Check if it was for closing
			if ((valread = read( sd , requestBody, 1024)) == 0)
			{
				//Somebody disconnected
				//Close the socket and mark as 0 in list for reuse
				close( sd );
				clients.erase(clients.begin() + i);
				FD_CLR(sd, &readfds);
			}
			// otherwise handle the request
			else
			{
				//set the string terminating NULL byte on the end
				// of the data we will read
				requestBody[valread] = '\0';
				servers[0].handleConnection(requestBody, sd);
				close(sd);
				clients.erase(clients.begin() + i);
				FD_CLR(sd, &readfds);
			}
		}
	}
}

// void	Server::server_loop()
// {
// 	//set of socket descriptors
// 	fd_set				readfds;
// 	std::vector<Socket> clients;
// 	Socket				new_socket;
// 	int					max_sd;

// 	while(TRUE)
// 	{
// 		//clear the sockets set
// 		FD_ZERO(&readfds);
// 		// add all servers sockets to the sockets set  [readfds]
// 		Server::addServers(serversSockets, max_sd, readfds);
// 		// add child sockets to the sockets set
// 		Server::addClients(clients, max_sd, readfds);
// 		// wait for an activity on one of the client sockets , timeout is NULL ,
// 		// so wait indefinitely
// 		Server::waitingForConnections(activity, readfds);
// 		// If something happened on the servers sockets ,
// 		// then its an incoming connection
// 		Server::acceptNewConnection(clients, serversSockets, address, addrlen, readfds);
// 		// otherwise its some IO operation on some other socket
// 		// recieve Client Request And Send Back A Response();
// 		Server::RecvAndSend(clients, readfds, servers);
// 	}
// }