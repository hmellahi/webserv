/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamza <hamza@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/15 16:14:21 by hamza             #+#    #+#             */
/*   Updated: 2021/10/16 01:22:05 by hamza            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Enums.hpp"
#include "Request.hpp"
#include "Response.hpp"

class Server
{
private:
    int _serverFd;
    struct sockaddr_in _address;
    int addrlen;
    int _port;

public:
    Server(int port)
    {
        _port = port;
        addrlen = sizeof(_address);
        _address.sin_family = AF_INET;
        _address.sin_addr.s_addr = INADDR_ANY;
        _address.sin_port = htons( _port );
        memset(_address.sin_zero, '\0', sizeof _address.sin_zero);
    }

    ~Server()
    {
        close(_serverFd);
    }
    
    int     init()
    {
        // Creating server socket file descriptor
        if ((_serverFd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
        {
            perror("In socket");
            return (3);
        }
        if (bind(_serverFd, (struct sockaddr *)&_address, sizeof(_address))<0)
        {
            perror("In bind");
            return (4);
        }
        if (listen(_serverFd, 10) < 0)
        {
            perror("In listen");
            return (5);
        }
        std::cout << "Server started, go to 127.0.0.1:" << _port << std::endl;
        return (0);
    }

    void start()
    {
        int client_fd;
        int ret;
        
        while(1)
        {
            printf("\n+++++++ Waiting for new connection ++++++++\n\n");
            
            if ((client_fd = accept(_serverFd, (struct sockaddr *)&_address, (socklen_t*)&addrlen))<0)
            {
                perror("In accept");
                exit(EXIT_FAILURE);
            }
                        
            char buffer[30000] = {0};
            ret = read( client_fd , buffer, 30000);
            Request req(buffer);
            Response res(req, client_fd);
            handleRequest(req, res);

            printf("------------------Hello message sent-------------------");
            
            close(client_fd);
        }
    }

    void    handleRequest(Request req, Response res)
    {
        // todo
        // check which method [GET, POST, DELETE]
        // todo
        // check if the path is a directory
        // if so then check if there is any default pages (index.html index ect..)
        // other wise show permission denied
        int error = OK;
        std::string buffer = readFile(req.getPath(), error);
        if (error != OK)
            return res.send(error, getErrorPageContent(error), true);
        return res.send(OK, buffer);
    }

    std::string     readFile(std::string filename, int &error)
    {
        struct stat info;
        if ((stat(filename.c_str(), &info)) == -1)
        {
            error = NOT_FOUND;
            return (NULL);
        }
        
        std::ifstream  file(filename);
        if (!file)
        {
            error = PERMISSION_DENIED;
            return (NULL);
        }
        
        std::string buffer, line;
        while ( getline (file,line) )
        {
            buffer += line;
            buffer += "\n";
        }
        file.close();
        return (buffer);
    }

    std::string     getErrorPageContent(int status_code)
    {
		// todo
        // check if there is a custom error page 
		

		// otherwise make one
		std::string html;
        std::string reponseMsg = Response::getResponseMsg(status_code);
		html = "<html>"
				"<head><title>" + reponseMsg +"</title></head>"
				"<body>"
				"<center><h1>" + reponseMsg + "</h1></center>"
				"<hr><center>Web server dyal lay7sn l3wan/1.18.0 (Ubuntu)</center>"
				"</body>"
				"</html>";
		return (html);
    }
};