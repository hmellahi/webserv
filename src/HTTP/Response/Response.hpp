#pragma once
#include "Server.hpp"
#include "MediaTypes.h"
#include "Utils.hpp"

class Response
{
private:
    int _method;
    Config _serverConfig;
    std::map<std::string, std::string> _headers;

public:
    Response();
    int _client_fd;
    Response(const Response& src);
    Response(Request req, int client_fd, Config serverConfig );
     
    void    send( int statusCode);
    void    send( int status_code, std::string filename);
    void    sendRedirect(int statusCode, const std::string & location);
    int     sendMessage(int fd, const std::string &s);
    int     sendRaw(int fd, const void *buf, int buflen);
    void    readRaw(std::string buffer, int FileLength);
    std::string     CraftRedirectionPage(int statusCode);

    std::string getHeader(std::string header_name);
    std::map<std::string, std::string>  getHeaders() const;
    int     getClientFd() const;
    Config  getServerConfig() const;
    void setHeader(std::string header_name, std::string value);
};