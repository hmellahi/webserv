/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamza <hamza@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/12 22:11:00 by hamza             #+#    #+#             */
/*   Updated: 2021/10/17 03:14:06 by hamza            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// Server side C program to demonstrate HTTP Server programming
#include "Server.hpp"
#include "CGI.hpp"


int main(int ac, char const *av[])
{
    (void)ac;
    // char path[] = "tests/index.php";
    // std::cout << CGI::exec_file(path);
    Server nginx;
    nginx.listen(atoi(av[1]));
    nginx.start();
    return 0;
}