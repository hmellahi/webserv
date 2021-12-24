#pragma once
#include "Server.hpp"
#include "MediaTypes.h"
#include "Utils.hpp"

class Response
{
private:
    Config _serverConfig;
    std::map<std::string, std::string> _headers;
    

public:
    std::string _msg;
    int nbytes_left;
    int file_to_send;
    Response();
    int _client_fd;

    Response(const Response& src);
    Response(Request req, int client_fd, Config serverConfig );
     
    Response    send( int statusCode);
    Response    send( int status_code, std::string filename);
    Response    sendContent( int status_code, std::string content);
    Response    sendRedirect(int statusCode, const std::string & location);
    int     sendMessage(int fd, const std::string &s);
    int     sendRaw(int fd, const void *buf, int buflen);
    std::string readRaw(int fd, int fileLength, int &bytes_read);

    std::string getHeader(std::string header_name);
    std::map<std::string, std::string>  getHeaders() const;
    int     getClientFd() const;
    Config  getServerConfig() const;
    void setHeader(std::string header_name, std::string value);
};
 