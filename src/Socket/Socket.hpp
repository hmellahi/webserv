/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamza <hamza@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 01:53:18 by hamza             #+#    #+#             */
/*   Updated: 2021/10/18 02:23:11 by hamza            ###   ########.fr       */
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

    void    init_address();
    void    setup_socket();

public:
    Socket(int port); // todo add more params

    int getSocketFd() const { return _socket_fd; };
};