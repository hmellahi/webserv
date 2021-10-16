/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmellahi <hmellahi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/15 17:14:51 by hamza             #+#    #+#             */
/*   Updated: 2021/10/16 21:53:06 by hmellahi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Server.hpp"
#include "MimeTypes.h"
#include "str_utils.hpp"

class Response
{
private:
    int _method;
    int _client_fd;
    std::string _path;
    std::string _contentType;
    std::map<std::string, std::string> _headers;

public:

    Response(Request request, int client_fd)
    {
        _client_fd = client_fd;
        // _headers = request.getHeaders(); todo
        // headers :
        // connection : close/keep-alive
        // url 
        // method 
        // http version 
        // std::cout << MimeTypes::getType("pdf") << std::endl;
        // _headers["url"] = "tests/s_web/index.html"; // temp
        _headers["url"] = "tests/s_web/index.html"; // temp
        _headers["Content-Type"] = MimeTypes::getType(GetFileExtension(_headers["url"]).c_str()); // todo fix seg
        std::cout << "type: [" << _headers["Content-Type"] << std::endl;
        // _headers["Content-Type"] = "text/html";
    }
    
    void    send( int status_code, std::string buffer, bool isErrorPage = false)
    {
        std::string responseText;

        if (isErrorPage)
            _headers["Content-Type"] = "text/html"; 
        responseText = "HTTP/1.1 " + getResponseMsg(status_code) + "\nContent-Type: "+ _headers["Content-Type"] + "\nContent-Length: ";
        responseText += buffer.size();
        responseText += "\n\n";
        responseText += buffer;

        std::cout << write(_client_fd, responseText.c_str(), strlen(responseText.c_str())) << std::endl;
        // char* responseText;

        // if (isErrorPage)
        //     _headers["Content-Type"] = "text/html"; 
        // responseText = "HTTP/1.1 " + getResponseMsg(status_code) + "\nContent-Type: "+ _headers["Content-Type"] + "\nContent-Length: ";
        // responseText += buffer.size();
        // responseText += "\n\n";
        // responseText += buffer;
        // write(_client_fd, responseText, strlen(responseText));
    }
    
    static std::string getResponseMsg(int status_code)
    {
        std::map<int, std::string> responseMessages;

        // Successful responses
        responseMessages[OK] = "200 OK";
        // responseMessages[202] = "";
        
        // CLient side error response
        responseMessages[NOT_FOUND] = "404 Not Found";
        responseMessages[402] = "402";
        responseMessages[405] = "405 Method Not Allowed";
        
        // Server Errors
        responseMessages[BAD_GATEWAY] = "502 Bad Gateway";
        
        return (responseMessages[status_code]);
    }

    std::string getHeader(std::string header_name)
    {
        return _headers[header_name];
    }
};