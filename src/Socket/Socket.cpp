/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamza <hamza@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 02:04:00 by hamza             #+#    #+#             */
/*   Updated: 2021/10/18 02:18:11 by hamza            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"

Socket::Socket(int port)
{
    _port = port;
    
    // initialize adress
    init_address();
    // setup socket
    setup_socket();
}

void    Socket::init_address()
{
    addrlen = sizeof(_address);
    _address.sin_family = AF_INET;
    _address.sin_addr.s_addr = INADDR_ANY;
    _address.sin_port = htons( _port );
    memset(_address.sin_zero, '\0', sizeof _address.sin_zero);
}

void    Socket::setup_socket()
{
    // Creating server socket file descriptor
    if ((_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("In socket");
        exit(1);
    }
    //bind the socket to localhost port
    if (bind(_socket_fd, (struct sockaddr *)&_address, addrlen) < 0)
    {
        perror("In bind");
        exit(1);
    }
    //  specify maximum of 10 pending connections for this socket
    if (listen(_socket_fd, 10) < 0)
    {
        perror("In listen");
        exit(1);
    }
}    