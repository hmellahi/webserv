/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmellahi <hmellahi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/15 16:14:21 by hamza             #+#    #+#             */
/*   Updated: 2021/10/20 18:43:00 by hmellahi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "macros.hpp"
#include "Config.hpp"
#include "Request.hpp"
#include "CGI.hpp"
#include "Response.hpp"
#include "../utils/FileSystem/FileSystem.hpp"
#include "Socket.hpp"

class Server;

typedef void  (Server::*methodType)(Request, Response);

class Server
{
private:
    int _serverFd;
    std::vector<Socket> _serverSockets;
    struct sockaddr_in _address;
    int addrlen;
    int _port;

public:
    Server()
    {
    } 

    ~Server()
    {
        // for (int i = 0; i < _serverSocketsFd.size(); i++)
        //     close(_serverSocketsFd[i]);
    }
    
    static void	addClients(std::vector<Socket> clients, int &max_fd, fd_set &readfds);
    static void	waitingForConnections(int &activity, fd_set &readfds);
    static void	addServers(std::vector<Socket> &sockets, int &max_sd, fd_set &readfds);
    static void	acceptNewConnection(std::vector<Socket> &clients, std::vector<Socket> &serversSockets, struct sockaddr_in &address, int &addrlen, fd_set &readfds);
    static void	RecvAndSend(std::vector<Socket> &clients, fd_set &readfds,  std::vector<Server> &servers);




    std::vector<Socket> getSockets()
    {
       return _serverSockets;
    }
    
    void    addPort(int port)
    {
        // create new socket on the given port
        Socket new_socket(port);
        // save socker fd
        _serverSockets.push_back(new_socket);
        // debugging
        std::cout << "Server started, go to 127.0.0.1:" << port << std::endl;
    }

    void    handleConnection(std::string requestBody, int client_fd)
    {
        Request req(requestBody);
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
        int status; // this will be filled by file status after passing it to readFile()
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
        // Config conf; // todo remove this
        // int status;
        // // std::map<int, std::string> errorPages = conf.get();
        // std::string filename = errorPages[status_code];
        // if (!filename.empty())
        //     return (FileSystem::readFile(filename, status));

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