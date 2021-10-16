/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmellahi <hmellahi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/12 22:11:00 by hamza             #+#    #+#             */
/*   Updated: 2021/10/16 21:32:13 by hmellahi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// Server side C program to demonstrate HTTP Server programming
#include "Server.hpp"


int main(int ac, char const *av[])
{
    (void)ac;
    Server nginx(atoi(av[1]));
    nginx.init();
    nginx.start();
    // std::cout << "type: " << GetFileExtension("tests/s_web/img.jpg").c_str() << std::endl;
    return 0;
}