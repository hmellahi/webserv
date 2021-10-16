/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmellahi <hmellahi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/15 16:14:21 by hamza             #+#    #+#             */
/*   Updated: 2021/10/16 21:51:34 by hmellahi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "macros.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "../utils/FileSystem/FileSystem.hpp"

class Server;

typedef void  (Server::*methodType)(Request, Response);

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
            std::cout << buffer << std::endl;
            Request req(buffer);
            Response res(req, client_fd);
            handleRequest(req, res);

            printf("------------------Hello message sent-------------------");
            
            close(client_fd);
        }
    }

    void    handleRequest(Request req, Response res)
    {
        (this->*getMethodHandler(req.getMethod()))(req, res);
    }

    methodType    getMethodHandler(int methodIndex)
    {
        std::map<int, methodType>methodsHandler;
        
        methodsHandler[GET] = &Server::getHandler;
        methodsHandler[POST] = &Server::postHandler;
        methodsHandler[DELETE] = &Server::deleteHandler;
        
        return (methodsHandler[methodIndex] ? methodsHandler[methodIndex] : &Server::methodNotFoundHandler);
    }

    void    getHandler(Request req, Response res)
    {
        // todo
        // check if the path is a directory
        // if so then check if there is any default pages (index.html index ect..)
        // otherwise if autoindex is On list directory files
        // otherwise show error page
        int status = OK;
        std::string buffer = FileSystem::readFile(res.getHeader("url"), status);
        if (status != OK)
            return res.send(status, getErrorPageContent(status), true);
        return res.send(OK, buffer);
    }
    
    void    postHandler(Request req, Response res)
    {
         // todo
    }
 
    void    deleteHandler(Request req, Response res)
    {
         // todo
    }

    void    methodNotFoundHandler(Request req, Response res)
    {
        // todo
    }

    std::string     getErrorPageContent(int status_code)
    {
		// todo
        // check if there is a custom error page 
		

		// otherwise craft one
		std::string html;
        std::string reponseMsg = Response::getResponseMsg(status_code);
		html = "<html>"
				"<head><title>" + reponseMsg +"</title></head>"
				"<body>"
				"<center><h1>" + reponseMsg + "</h1></center>"
				"<hr><center>Web server dyal lay7sn l3wan/1.18.0 (Ubuntu)</center>"
				"</body>"
				"</html>";
        std::cout << html <<std::endl;
		return (html);
    }
};