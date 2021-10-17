/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmellahi <hmellahi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/12 22:11:00 by hamza             #+#    #+#             */
/*   Updated: 2021/10/17 20:26:21 by hmellahi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// Server side C program to demonstrate HTTP Server programming
#include "Server.hpp"
#include "CGI.hpp"
#include "Config.hpp"


int main(int ac, char const *av[])
{
    (void)ac;
    // char path[] = "tests/index.php";
    // std::cout << CGI::exec_file(path);
    // Server nginx;
    // nginx.listen(atoi(av[1]));
    // nginx.start();
    // Get Server
    Config nginx;
    nginx._ports.push_back(atoi(av[1]));
    // Config apache;
    // apache._ports.push_back(atoi(av[1]) + 1);
    std::vector<Config> serversList;
    serversList.push_back(nginx);
    // serversList.push_back(apache);
    Config server;
    std::vector<int> ports;
    
    // loop throught all Servers
    for (std::vector<Config>::iterator it = serversList.begin(); it != serversList.end(); it++)
    {
        server = *it;
        ports = server._ports;
        Server newServer;
        for (int i = 0; i < ports.size(); i++)
            newServer.listen(ports[i]);
        newServer.start();
    }
    return 0;
}