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
#include "ParseConfig.hpp"
#include "Socket.hpp"

#define TRUE 1
#define FALSE 0
#define PORT 8888
#define MAX_CLIENTS FD_SETSIZE

void	server_loop(std::vector<Socket> &serversSockets, std::vector<Server> &servers)
{
	//set of socket descriptors
	fd_set				readfds;
	std::vector<Socket> clients;
	Socket				new_socket;
	int					max_sd;
	int  addrlen , activity;
	struct sockaddr_in address;

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
}

void read_config(ParseConfig &ParseConfig)
{
	std::vector<Config> servers = ParseConfig.getServers();
	std::vector<Config>::iterator it;
	std::map<std::string, Config>::iterator it_loc;
	for (it = servers.begin(); it != servers.end(); it++)
	{
		std::cout << "-------------  Server Information    -------------\n";
		std::cout <<"Root:" << it->getRoot() << std::endl;
		std::cout <<"Server Name: ";
		std::vector<std::string> server_names = it->get_server_name();
		for (std::vector<std::string>::iterator sn = server_names.begin(); sn != server_names.end(); sn++)
		{
			std::cout  << *sn << " ";
		}
		std::cout << "\n";
		std::cout <<"Max Body:" << it->get_client_max_body_size() << std::endl;
		std::cout <<"Auto Index:" << it->get_isAutoIndexOn() << std::endl;
		std::map<std::string, Config> loc = it->getLocation();
		for (it_loc = loc.begin(); it_loc != loc.end(); it_loc++)
		{
			std::cout << "In locations: Root: " << it_loc->second.getRoot() << " || upload_path: " << it_loc->second.get_uploadPath() << std::endl;
			std::cout <<"-----> Server Name: ";
			std::vector<std::string> server_names = it_loc->second.get_server_name();
			for (std::vector<std::string>::iterator sn = server_names.begin(); sn != server_names.end(); sn++)
			{
				std::cout  << *sn << " ";
			}
			std::cout << "\n";
		}
		std::cout << "------------- End Server Information -------------\n\n";

	}
}

void	setup_servers(ParseConfig GlobalConfig)
{
	std::vector<Config> serversConfigs = GlobalConfig.getServers();
	std::vector<Config>::iterator serverConfig;
	std::vector<u_int32_t>::iterator port;
	std::vector<Socket> serversSockets;
	std::vector<Server> servers;
	Socket new_socket;

	for (serverConfig = serversConfigs.begin(); serverConfig != serversConfigs.end(); serverConfig++)
	{
		Server newServer(*serverConfig);

		std::vector<u_int32_t> ports = serverConfig->get_listen();
		for (port = ports.begin(); port != ports.end(); port++)
		{
			new_socket = newServer.addPort(*port);
			serversSockets.push_back(new_socket);
			std::cout << new_socket.getSocketFd() << std::endl;
		}
		servers.push_back(newServer);
	}
	server_loop(serversSockets, servers);
}

int main(int ac , char *av[])
{
	// try
	// {
	// 	if (ac <= 2)
	// 	{
	// 		const char * file;

	// 		if (av[1])
	// 			file = av[1];
	// 		else
	// 			file = DEFAULT_CONFIG_PATH;
	// 		// 1 - first thing to do is parsing the config file
	// 		ParseConfig GlobalConfig(file);
	// 		// read_config(ParseCo nfig);
	// 		start_servers(GlobalConfig);
	// 	}
	// 	else
	// 	{
	// 		std::cerr << "Error: Wrong Number of Argument ." << std::endl;
	// 		exit(EXIT_FAILURE);
	// 	}
	// }
	// catch(const std::exception& e)
	// {
	// 	std::cerr << e.what() << '\n';
	// }
	// int  addrlen , activity ;
	// struct sockaddr_in address;
	// char buffer[1025];

	Server	nginx;
	nginx.addPort(atoi(av[1]));
	nginx.addPort(atoi(av[1]) + 1);

	Server	apache;
	apache.addPort(atoi(av[1]) + 2);
	apache.addPort(atoi(av[1]) + 3);

	std::vector<Server> servers;
	servers.push_back(nginx);
	servers.push_back(apache);
	std::vector<Socket> serversSockets;
	std::vector<Socket> nginxFds = nginx.getSockets();
	std::vector<Socket> apacheFds = apache.getSockets();	

	serversSockets.insert(serversSockets.begin(), nginxFds.begin(), nginxFds.end());
	serversSockets.insert(serversSockets.begin(), apacheFds.begin(), apacheFds.end());


	//accept the incoming connection
	// addrlen = sizeof(address);
	server_loop(serversSockets, servers);
	return 0;
}