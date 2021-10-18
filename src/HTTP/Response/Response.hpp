/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamza <hamza@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/15 17:14:51 by hamza             #+#    #+#             */
/*   Updated: 2021/10/18 04:15:31 by hamza            ###   ########.fr       */
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
        _headers["http-version"] = req.getHeader("http-version");//
        _headers["Connection"] = "close";
        _headers["Date"] = getCurrentDate(); 
        // std::cout << "date: " << _headers["Date"] << std::endl;
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

        struct stat info;
        stat(buffer.c_str(), &info);
        off_t fileLength = info.st_size;
        std::ostringstream msg;
        int fd = open(buffer.c_str(), O_RDONLY);
        
        // todo
        // add all mandatory response headers
        msg << _headers["http-version"] << " " << status_code << " " 
            << HttpStatus::reasonPhrase(status_code) << "\r\n"
            << "Content-Type: " << _headers["Content-Type"] << "\r\n"
            << "Content-Length: " << fileLength << "\r\n"
            << "Connection: " << _headers["Connection"] << "\r\n"
            << "Date: " << _headers["Date"] << "\r\n"
            << "\r\n";
        
        sendMessage(_client_fd, msg.str());
        #define BUFSIZE 100
        char buf[BUFSIZE];
        int bytes_read, bytes_written;
        while (fileLength > 0) {
            bytes_read = read(fd, buf, BUFSIZE);//std::min((int)fileLength, BUFSIZE));
            if (bytes_read <= 0) break;
            if (sendRaw(_client_fd, buf, bytes_read) == -1) break;
            fileLength -= bytes_read;  
        }
        close(fd);
        std::cout <<"yaaaaaaaaaaaay" << std::endl;
        // std::cout << write(_client_fd, msg.c_str(), msg.length()) << std::endl;
    }

    int sendMessage(int fd, const std::string &s)
    {
        return sendRaw(fd, s.c_str(), s.length());
    }

    int sendRaw(int fd, const void *buf, int buflen)
    {
        const char *pbuf = static_cast<const char*>(buf);
        int bytes_written;

        while (buflen > 0) {
            bytes_written = write(fd, pbuf, buflen);
            // std::cout << pbuf << std::endl;
            if (bytes_written == -1) return -1;
            pbuf += bytes_written; 
            buflen -= bytes_written;
        }

        return 0;
    }

    std::string getHeader(std::string header_name)
    {
        return _headers[header_name];
    }

    void setHeader(std::string header_name, std::string value)
    {
        _headers[header_name] = value;
    }
};