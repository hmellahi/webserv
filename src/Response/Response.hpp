/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamza <hamza@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/15 17:14:51 by hamza             #+#    #+#             */
/*   Updated: 2021/10/16 01:12:26 by hamza            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Server.hpp"

class Response
{
private:
    int _method;
    int _client_fd;
    std::string _path;
    std::string _contentType;
    

public:

    Response(Request request, int client_fd)
    {
        _method = request.getMethod();
        _path   = request.getPath();
        _client_fd = client_fd;
        _contentType = "text/html";
    }
    
    void    send( int status_code, std::string buffer, bool isErrorPage = false)
    {
        std::string responseText;

        if (isErrorPage)
            res._contentType = "text/html";
        responseText = "HTTP/1.1 " + getResponseMsg(status_code) + "\nContent-Type: "+ _contentType + "\nContent-Length: ";
        responseText += buffer.size();
        responseText += "\n\n";
        responseText += buffer;
        std::cout << "---------- response" << std::endl << responseText << std::endl;
        write(_client_fd, responseText.c_str(), strlen(responseText.c_str()));
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
};