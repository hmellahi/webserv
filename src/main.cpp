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
#define MAX_CLIENTS FD_SETSIZE


int main(int argc , char *av[])
{
	int opt = TRUE;
	int  addrlen , activity, i , valread , sd;
	std::vector<Socket> clients;
	Socket new_socket;
	int max_sd;
	struct sockaddr_in address;
		
	char buffer[1025];

	//set of socket descriptors
	fd_set readfds;
		
	// Server	nginx;
	// nginx.addPort(atoi(av[1]));
	// nginx.addPort(atoi(av[1]) + 1);

	// Server	apache;
	// apache.addPort(atoi(av[1]) + 2);
	// apache.addPort(atoi(av[1]) + 3);

	// std::vector<Server> servers;
	// servers.push_back(nginx);
	// servers.push_back(apache);
	// std::vector<Socket> serversSockets;
	// std::vector<Socket> nginxFds = nginx.getSockets();
	// std::vector<Socket> apacheFds = apache.getSockets();

	// serversSockets.insert(serversSockets.begin(), nginxFds.begin(), nginxFds.end());
	// serversSockets.insert(serversSockets.begin(), apacheFds.begin(), apacheFds.end());


	//accept the incoming connection
	addrlen = sizeof(address);
	puts("Waiting for connections ...");
	while(TRUE)
	{
		//clear the sockets set
		FD_ZERO(&readfds);
		// add all servers sockets to the sockets set  [readfds]
		Server::addServers(serversSockets, max_sd, readfds);
		// add child sockets to the sockets set
		Server::addClients(clients, max_sd, readfds);
		// wait for an activity on one of the client sockets , timeout is NULL ,
		// so wait indefinitely
		Server::waitingForConnections(activity, readfds);
		// If something happened on the servers sockets ,
		// then its an incoming connection
		Server::acceptNewConnection(clients, serversSockets, address, addrlen, readfds);
		// otherwise its some IO operation on some other socket
		// recieve Client Request And Send Back A Response();
		Server::RecvAndSend(clients, readfds, servers);
	}
	return 0;
}
