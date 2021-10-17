/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamza <hamza@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/15 17:14:51 by hamza             #+#    #+#             */
/*   Updated: 2021/10/17 06:02:32 by hamza            ###   ########.fr       */
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

    Response(Request req, int client_fd)
    {
        _client_fd = client_fd;
        // _headers = request.getHeaders(); todo
        // headers :
        // connection : close/keep-alive
        // url 
        // http version 
        _headers["url"] = req.getHeader("url");
        _headers["http-version"] = req.getHeader("http-version");//"HTTP/1.1"
        // std::cout << "type: [" << _headers["Content-Type"] << std::endl;
    }
    
    void    send( int status_code, std::string buffer, bool isErrorPage = false)
    {
        std::string responseText;
        std::string extension;

        // std::cout << "header" << _headers["url"] << std::endl;
        if (isErrorPage)
            _headers["Content-Type"] = "text/html";
        else
        {
            extension = GetFileExtension(_headers["url"]);
            if (!extension.empty())
                _headers["Content-Type"] = MimeTypes::getType(extension.c_str());
            else
                _headers["Content-Type"] = "text/plain"; // todo fix seg
        }
        // todo refactor this shitty code
        // instead of storing strings store ints[enums]
        // add Date 
        responseText = _headers["http-version"] + " ";
        responseText += HttpStatus::reasonPhrase(status_code);
        responseText += "\nContent-Type: "+ _headers["Content-Type"];
        responseText += "\nContent-Length: ";
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
    
    // static std::string getResponseMsg(int status_code)
    // {
    //     std::cout << "statusCode: " << status_code << std::endl;
    //     std::map<int, std::string> responseMessages;

    //     // Successful responses
    //     responseMessages[HttpStatus::OK] = "200 HttpStatus::OK";
    //     // responseMessages[202] = "";
        
    //     // CLient side error response
    //     responseMessages[HttpStatus.] = "404 Not Found";
    //     responseMessages[PERMISSION_DENIED] = "403 Forbidden";
    //     responseMessages[METHOD_NOT_ALLOWED] = "405 Method Not Allowed";
        
    //     // Server Errors
    //     responseMessages[BAD_GATEWAY] = "502 Bad Gateway";
        
    //     return (responseMessages[status_code]);
    // }

    std::string getHeader(std::string header_name)
    {
        return _headers[header_name];
    }

    void setHeader(std::string header_name, std::string value)
    {
        _headers[header_name] = value;
    }
};