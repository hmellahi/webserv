/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmellahi <hmellahi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 01:53:18 by hamza             #+#    #+#             */
/*   Updated: 2021/10/20 15:42:06 by hmellahi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "macros.hpp"

class Socket
{
private:
    int _port;
    struct sockaddr_in _address;
    int addrlen;
    int _socket_fd;
    int _domain;    
    int _type;
    int _host;
    int _protocol;

    void    init_address();
    void    setup_socket();

public:
    // constructors
    // Socket(int socket_fd);
    Socket();
    Socket(int port, int host = INADDR_ANY, int domain = AF_INET, int type = SOCK_STREAM, int protocol = 0);
    
    // methods
    static void testConnection(int connection_ret, std::string customErrMsg = "Connection failed");
    static Socket    acceptConnection(int socket, struct sockaddr_in address, int addrlen);
    
    // Getters
    int getSocketFd() const;
    void setSocketFd(int socket_fd);

    operator int() const
    {
        return _socket_fd;
    } 
};