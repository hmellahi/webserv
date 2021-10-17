/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileSystem.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamza <hamza@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/16 21:38:31 by hmellahi          #+#    #+#             */
/*   Updated: 2021/10/17 03:15:25 by hamza            ###   ########.fr       */
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
    if ((int)((info.st_mode & S_IFDIR) == S_IFDIR))
    {
        status = IS_DIRECTORY;
        return ("");
    }
    // std::ifstream  file(filename);
    int fd = open(filename.c_str(), O_RDONLY);
    if (fd < 1)
    {
        status = PERMISSION_DENIED;
        return ("");
    }
    char buffer[30000] = {0};
    int ret = read(fd, buffer, 30000);
    // std::string buffer, line;
    // while ( getline (file,line) )
    // {
    //     buffer += line;
    //     buffer += "\n";
    //     std::cout << "line:[" << line << "]"  std::endl;
    // }
    
    close(fd);
    status = OK;
    // std::cout << fd << "|buffer:"<< buffer << std::endl;
    return (buffer);
}

std::string FileSystem::getIndexFileContent(std::string directory, std::string & filename)
{
    std::vector<std::string> indexFiles;
    std::string content;
    int status = !OK;

    indexFiles.push_back("index.html");
    indexFiles.push_back("index.php"); // todo remove this dummy shit
    // std::cout << 
    // for (std::vector<std::string>::iterator it = _indexFiles.begin(); status != OK && it != _indexFiles.end(); it++)
    for (int i = 0; i < indexFiles.size() && status != OK; i++)
    {
        std::cout << status << "|" << directory + indexFiles[i] << std::endl;
        content = readFile(directory + indexFiles[i], status);
        if (status == OK)
            filename = indexFiles[i];
    }
    return content;
}