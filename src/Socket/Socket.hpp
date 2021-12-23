#pragma once
#include "macros.hpp"
#include "Utils.hpp"

class Socket
{
private:
    int _port;
    struct sockaddr_in _address;
    int addrlen;
    int _socket_fd;
    int _domain;    
    int _type;
    std::string _host;
    int _protocol;

    void    init_address();
    void    create_socket();

public:
    int _lastuse;
    int type;
    // constructors
    // Socket(int socket_fd);
    Socket();
    Socket(int socket_fd);
    Socket(int port, std::string &host, int domain = AF_INET, int type = SOCK_STREAM, int protocol = 0);
    
    // methods
    static bool testConnection(int connection_ret, std::string customErrMsg = "Connection failed", bool suspend = true);
    static Socket    acceptConnection(int socket, struct sockaddr_in address, int addrlen);
    
    // Getters
    int getSocketFd() const;
    void setSocketFd(int socket_fd);

    operator int() const
    {
        return _socket_fd;
    } 
};