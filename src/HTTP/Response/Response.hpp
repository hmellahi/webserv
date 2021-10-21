#pragma once
#include "Server.hpp"
#include "MediaTypes.h"
#include "Utils.hpp"

class Response
{
private:
    int _method;
    int _client_fd;
    std::string _path;
    Config _serverConfig;
    std::string _contentType;
    std::map<std::string, std::string> _headers;

public:

    Response(Request req, int client_fd, Config serverConfig );
     
    void    send( int statusCode);
    void    send( int status_code, std::string filename);
    int     sendMessage(int fd, const std::string &s);
    int     sendRaw(int fd, const void *buf, int buflen);
    void    readRaw(std::string buffer, int FileLength);

    std::string getHeader(std::string header_name);
    void setHeader(std::string header_name, std::string value);
};