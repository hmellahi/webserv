/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamza <hamza@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/17 01:45:50 by hamza             #+#    #+#             */
/*   Updated: 2021/10/17 02:46:08 by hamza            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGI.hpp"

std::string     CGI::exec_file(char* path)
{
    char exec_path[] = "ls";
    char exec_path2[] = "-la";
    char *args[] = { exec_path, exec_path2 , 0};
    int fd[2];
    int pid;
    char* output;
    if (pipe(fd) < -1)
        exit(1);
    pid = fork();
    if (pid == -1)
        exit(1);
    if (pid == 0)
    {
        dup2(fd[1], 1);
        // close(fd[1]);
        close(fd[0]);
        // todo
        // set environment vars
        execve(args[0], args, NULL);
        // std::cout << "faiiil" << std::endl;
        // exit(0);
    }

    else
    {
        wait(NULL);
        // while (wait(NULL) != -1)
        // ;
        int nbytes = read(fd[0], output, 2000);
        printf("%s\n", output);
        printf("[%d]\n", nbytes);
        close(fd[1]);
        close(fd[0]);

        std::cout << output << std::endl;
    }
    return (output);
}