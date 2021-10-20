//Example code: A simple server side code, which echos back the received message.
//Handle multiple socket connections with select and fd_set on Linux
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
#include "Server.hpp"
#include "CGI.hpp"
#include "Config.hpp"
#include "ParseConfig.hpp"
#include "Socket.hpp"

int main(int ac , char *av[])
{
	// try
	// {
	// 	if (ac <= 2)
	// 	{
	// 		char *file = av[1] ? av[1] : DEFAULT_CONFIG_PATH;
	// 		// todo check if the file exits
	// 		// 1 - first thing to do is parsing the config file
	// 		ParseConfig GlobalConfig(file);
	// 		setup(GlobalConfig);
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
	Server::loop(serversSockets, servers);
	return 0;
}



// void read_config(ParseConfig &ParseConfig)
// {
// 	std::vector<Config> servers = ParseConfig.getServers();
// 	std::vector<Config>::iterator it;
// 	std::map<std::string, Config>::iterator it_loc;
// 	for (it = servers.begin(); it != servers.end(); it++)
// 	{
// 		std::cout << "-------------  Server Information    -------------\n";
// 		std::cout <<"Root:" << it->getRoot() << std::endl;
// 		std::cout <<"Server Name: ";
// 		std::vector<std::string> server_names = it->get_server_name();
// 		for (std::vector<std::string>::iterator sn = server_names.begin(); sn != server_names.end(); sn++)
// 		{
// 			std::cout  << *sn << " ";
// 		}
// 		std::cout << "\n";
// 		std::cout <<"Max Body:" << it->get_client_max_body_size() << std::endl;
// 		std::cout <<"Auto Index:" << it->get_isAutoIndexOn() << std::endl;
// 		std::map<std::string, Config> loc = it->getLocation();
// 		for (it_loc = loc.begin(); it_loc != loc.end(); it_loc++)
// 		{
// 			std::cout << "In locations: Root: " << it_loc->second.getRoot() << " || upload_path: " << it_loc->second.get_uploadPath() << std::endl;
// 			std::cout <<"-----> Server Name: ";
// 			std::vector<std::string> server_names = it_loc->second.get_server_name();
// 			for (std::vector<std::string>::iterator sn = server_names.begin(); sn != server_names.end(); sn++)
// 			{
// 				std::cout  << *sn << " ";
// 			}
// 			std::cout << "\n";
// 		}
// 		std::cout << "------------- End Server Information -------------\n\n";

// 	}
// }