/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamza <hamza@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/15 16:14:21 by hamza             #+#    #+#             */
/*   Updated: 2021/10/17 06:45:23 by hamza            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "macros.hpp"
#include "Config.hpp"
#include "Request.hpp"
#include "CGI.hpp"
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
    Server()
    {
    } 

    ~Server()
    {
        close(_serverFd);
    }
    
    int     listen(int port)
    {
        _port = port;
        addrlen = sizeof(_address);
        _address.sin_family = AF_INET;
        _address.sin_addr.s_addr = INADDR_ANY;
        _address.sin_port = htons( _port );
        memset(_address.sin_zero, '\0', sizeof _address.sin_zero);
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
        if (::listen(_serverFd, 10) < 0)
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
            handleRequest(req, client_fd);

            printf("--------------------------------------------------");
            
            close(client_fd);
        }
    }

    void    handleRequest(Request req, int client_fd)
    {
        Response res(req, client_fd);
        (this->*getMethodHandler(getMethodIndex(req.getHeader("method"))))(req, res);
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
        // check if the requested file isnt a static file
        // if so then pass it to CGI
        // otherwise jst read it
        std::string fileExtension = GetFileExtension(req.getHeader("url"));
        if (fileExtension == "php")
            return res.send(HttpStatus::OK, CGI::exec_file(req.getHeader("url").c_str()));
        // todo
        // check if the path is a directory
        // if so then check if there is any default pages (index.html index ect..)
        // otherwise if autoindex is On list directory files
        // otherwise show error page
        int status = HttpStatus::OK;
        std::string buffer = FileSystem::readFile(req.getHeader("url"), status);
        
        if (status == IS_DIRECTORY)
        {
            // check if one of the index files exists
            std::string fileName;
            std::string indexFileContent = FileSystem::getIndexFileContent(req.getHeader("url"), fileName);
            if (!indexFileContent.empty())
            {
                res.setHeader("url", req.getHeader("url") + fileName);
                return res.send(HttpStatus::OK, indexFileContent);
            }
            // otherwise
            // check if autoindex is on
            else
            {
                // if so then list all files in the current directory [soon]
                // if (Config::isAutoIndexOn())
                if (false)
                {
                    // SOON
                }
                // otherwise show permission denied page
                else
                    return res.send(HttpStatus::Forbidden, getErrorPageContent(HttpStatus::Forbidden), true);
            }
        }
        else if (status != HttpStatus::OK)
            return res.send(status, getErrorPageContent(status), true);
        
        return res.send(HttpStatus::OK, buffer);
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
        // check if there is a custom error page
        Config conf; // todo remove this
        int status;
        std::map<int, std::string> errorPages = conf.getCustomErrorPages();
        std::string filename = errorPages[status_code];
        if (!filename.empty())
            return (FileSystem::readFile(filename, status));

		// otherwise craft one
		std::string html;
        std::string reponseMsg = std::to_string(status_code) + " " + HttpStatus::reasonPhrase(status_code);
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

    int getMethodIndex(std::string method_name)
    {
        if (method_name == "GET")
            return GET;    
        else if (method_name == "POST")
            return POST;
        // else if (method_name == "DELETE")
        return DELETE;
        // todo handle unhandled methods ..
    }
};