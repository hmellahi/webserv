/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamza <hamza@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/12 22:11:00 by hamza             #+#    #+#             */
/*   Updated: 2021/10/15 19:00:53 by hamza            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// Server side C program to demonstrate HTTP Server programming
#include "Server.hpp"

int main(int ac, char const *av[])
{
    (void)ac;
    // (void)av;
    Server nginx(atoi(av[1]));
    nginx.init();
    nginx.start();
    return 0;
}