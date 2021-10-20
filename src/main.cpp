// /* ************************************************************************** */
// /*                                                                            */
// /*                                                        :::      ::::::::   */
// /*   main.cpp                                           :+:      :+:    :+:   */
// /*                                                    +:+ +:+         +:+     */
// /*   By: hamza <hamza@student.42.fr>                +#+  +:+       +#+        */
// /*                                                +#+#+#+#+#+   +#+           */
// /*   Created: 2021/10/12 22:11:00 by hamza             #+#    #+#             */
// /*   Updated: 2021/10/18 01:06:13 by hamza            ###   ########.fr       */
// /*                                                                            */
// /* ************************************************************************** */

// // Server side C program to demonstrate HTTP Server programming
// #include "Server.hpp"
// #include "CGI.hpp"
// #include "Config.hpp"


// int main(int ac, char const *av[])
// {
//     (void)ac;
//     // char path[] = "tests/index.php";
//     // std::cout << CGI::exec_file(path);
//     // Server nginx;
//     // nginx.listen(atoi(av[1]));
//     // nginx.start();
//     // Get Server
//     Config nginx;
//     nginx._ports.push_back(atoi(av[1]));
//     // Config apache;
//     // apache._ports.push_back(atoi(av[1]) + 1);
//     std::vector<Config> serversList;
//     serversList.push_back(nginx);
//     // serversList.push_back(apache);
//     Config server;
//     std::vector<int> ports;
    
//     // loop throught all Servers
//     for (std::vector<Config>::iterator it = serversList.begin(); it != serversList.end(); it++)
//     {
//         server = *it;
//         ports = server._ports;
//         Server newServer;
//         for (int i = 0; i < ports.size(); i++)
//             newServer.listen(ports[i]);
//         newServer.start();
//     }
//     return 0;
// }

//Example code: A simple server side code, which echos back the received message.
//Handle multiple socket connections with select and fd_set on Linux
#include <stdio.h>
#include <string.h> //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h> //close
#include <arpa/inet.h> //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
#include "Server.hpp"
#include "CGI.hpp"
#include "Config.hpp"
#include "Socket.hpp"

#define TRUE 1
#define FALSE 0
#define PORT 8888
	
int main(int argc , char *av[])
{
	int opt = TRUE;
	int  addrlen , new_socket ,
		max_clients = FD_SETSIZE , activity, i , valread , sd;
	std::vector<int> clients_socket;
	int max_sd;
	struct sockaddr_in address;
		
	char buffer[1025]; //data buffer of 1K
		
	//set of socket descriptors
	fd_set readfds;
		
	Server	nginx;
	nginx.addPort(atoi(av[1]));
	nginx.addPort(atoi(av[1]) + 1);

	Server	apache;
	apache.addPort(atoi(av[1]) + 2);
	apache.addPort(atoi(av[1]) + 3);

	std::vector<int> _serversSocketsFd;
	std::vector<int> nginxFds = nginx.getSocketsFd();
	std::vector<int> apacheFds = apache.getSocketsFd();

	_serversSocketsFd.insert(_serversSocketsFd.begin(), nginxFds.begin(), nginxFds.end());
	_serversSocketsFd.insert(_serversSocketsFd.begin(), apacheFds.begin(), apacheFds.end());

	//accept the incoming connection
	addrlen = sizeof(address);
	puts("Waiting for connections ...");
	int fd;
	while(TRUE)
	{
		//clear the socket set
		FD_ZERO(&readfds);
	
		//add all servers sockets to set
		for (int i = 0; i < _serversSocketsFd.size(); i++)
			FD_SET(_serversSocketsFd[i], &readfds);
		max_sd = _serversSocketsFd[_serversSocketsFd.size() - 1];
			
		//add child sockets to set
		for ( i = 0 ; i < clients_socket.size() ; i++)
		{
			//socket descriptor
			sd = clients_socket[i];
				
			//if valid socket descriptor then add to read list
			if(sd > 0)
				FD_SET( sd , &readfds);
				
			//highest file descriptor number, need it for the select function
			if(sd > max_sd)
				max_sd = sd;
		}
		add_clients();
	
		//wait for an activity on one of the sockets , timeout is NULL ,
		//so wait indefinitely
		activity = select(FD_SETSIZE , &readfds , NULL , NULL , NULL);
		if ((activity < 0) && (errno!=EINTR))
	
		{
			printf("select error");
		}
			
		//If something happened on the master socket ,
		//then its an incoming connection
		for (int i = 0; i < _serversSocketsFd.size(); i++)
		{
			fd = _serversSocketsFd[i];
			if (FD_ISSET(fd, &readfds))
			{
				new_socket = Socket::acceptConnection(fd, address, addrlen);
				
				//inform user of socket number - used in send and receive commands
				// printf("New connection , socket fd is %d , ip is : %s , port : %d\n" , new_socket , inet_ntoa(address.sin_addr) , ntohs
				// 	(address.sin_port));

				// if position is empty
				if (clients_socket.size() < max_clients)
					clients_socket.push_back(new_socket);
			}
		}
			
		//else its some IO operation on some other socket
		for (i = 0; i < clients_socket.size(); i++)
		{
			sd = clients_socket[i];
				
			if (FD_ISSET( sd , &readfds))
			{ 
				//Check if it was for closing , and also read the
				//incoming message
				if ((valread = read( sd , buffer, 1024)) == 0)
				{
					//Somebody disconnected , get his details and print
					getpeername(sd , (struct sockaddr*)&address , \
						(socklen_t*)&addrlen);
					// printf("Host disconnected , ip %s , port %d \n" ,
					// 	inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
						
					//Close the socket and mark as 0 in list for reuse
					close( sd );
					clients_socket.erase(clients_socket.begin() + i);
					FD_CLR(sd, &readfds);
				}
					
				//Echo back the message that came in
				else
				{
					//set the string terminating NULL byte on the end
					//of the data read
					buffer[valread] = '\0';
					nginx.handleConnection(buffer, sd);
                    close( sd );
					clients_socket.erase(clients_socket.begin() + i);
					FD_CLR(sd, &readfds);
				}
			}
		}
	}
		
	return 0;
}
