/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmellahi <hmellahi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 02:04:00 by hamza             #+#    #+#             */
/*   Updated: 2021/10/20 15:49:10 by hmellahi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"

Socket::Socket()
{

}

// Socket::Socket(int socket_fd)
// {
//     _socket_fd = socket_fd;
// }
// Socket::Socket(const Socket &src)
// {
//     _socket_fd = ;
//     _port = src._port;
//     _domain = src._domain;
//     _type = src._type;
//     _host = src._host;
//     _protocol = src._protocol;
// }

Socket::Socket(int port, int host, int domain, int type, int protocol)
{
    _port = port;
    _domain = domain;    
    _type = type;
    _host = host;
    _protocol = protocol;
    // initialize adress
    init_address();
    // setup socket
    setup_socket();
}

void    Socket::init_address()
{
    addrlen = sizeof(_address);
    _address.sin_family = _domain;
    _address.sin_addr.s_addr = _host;
    _address.sin_port = htons( _port );
    memset(_address.sin_zero, '\0', sizeof _address.sin_zero);
}

void    Socket::setup_socket()
{
    // Creating server socket file descriptor
    _socket_fd = socket(_domain, _type, _protocol);
    testConnection(_socket_fd, "couldnt create a new socket");
    
    //bind the socket to the giving port
    bind(_socket_fd, (struct sockaddr *)&_address, addrlen);
    testConnection(_socket_fd, "the port is already in Use...");

    //  specify maximum of 10 pending connections for this socket
    listen(_socket_fd, 10);
    testConnection(_socket_fd, "couldnt listen");
}

void    Socket::testConnection(int connection_ret, std::string customErrMsg)
{
    if (connection_ret < 0)
    {
        perror(customErrMsg.c_str());
        exit(EXIT_FAILURE);
    }
}

Socket     Socket::acceptConnection(int socket, struct sockaddr_in address, int addrlen)
{
    Socket new_socket;
    int new_connection;

    new_connection = accept(socket, (struct sockaddr *)&address, (socklen_t*)&addrlen);
    testConnection(new_connection, "couldnt accept new connection");
    
    new_socket.setSocketFd(new_connection);
    
    return (new_socket);
}

// Getters
int Socket::getSocketFd() const{ return _socket_fd; }
void Socket::setSocketFd(int socket_fd){ _socket_fd = socket_fd;}