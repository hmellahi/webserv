/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileSystem.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmellahi <hmellahi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/16 21:38:31 by hmellahi          #+#    #+#             */
/*   Updated: 2021/10/16 21:52:41 by hmellahi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "FileSystem.hpp"

std::string     FileSystem::readFile(std::string filename, int &status)
{
    struct stat info;
    if ((stat(filename.c_str(), &info)) == -1)
    {
        status = NOT_FOUND;
        return ("");
    }
    
    std::ifstream  file(filename);
    if (!file)
    {
        status = PERMISSION_DENIED;
        return ("");
    }
    
    std::string buffer, line;
    while ( getline (file,line) )
    {
        buffer += line;
        buffer += "\n";
    }
    file.close();
    return (buffer);
}