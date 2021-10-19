/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileSystem.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamza <hamza@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/16 21:38:31 by hmellahi          #+#    #+#             */
/*   Updated: 2021/10/19 00:11:16 by hamza            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "FileSystem.hpp"
#include "Config.hpp"

std::string     FileSystem::readFile(std::string filename, int &status)
{
    struct stat info;
    if ((stat(filename.c_str(), &info)) == -1)
    {
        status = HttpStatus::NotFound;
        return ("");
    }
    off_t fileLength = info.st_size;
    // std::cout << "LENGHT: " <<fileLength << std::endl;
    if ((int)((info.st_mode & S_IFDIR) == S_IFDIR))
    {
        status = IS_DIRECTORY;
        return ("");
    }
    // std::ifstream  file(filename);
    // int fd = open(filename.c_str(), O_RDONLY);
    // if (fd < 1)
    // {
    //     status = HttpStatus::Forbidden;
    //     return ("");
    // }
    // char buffer[20000] = {0};
    // int ret = read(fd, buffer, 30000);
    // // std::string buffer, line;
    // // while ( getline (file,line) )
    // // {
    // //     buffer += line;
    // //     buffer += "\n";
    // //     std::cout << "line:[" << line << "]"  std::endl;
    // // }
    // std::cout << "len: [" << strlen(buffer) << "]\n";
    // close(fd);
    status = HttpStatus::OK;
    // std::cout << fd << "|buffer:"<< buffer << std::endl;
    // return (buffer);
    return (filename);
}

std::string FileSystem::getIndexFileContent(std::string directory, std::string & filename)
{
    Config conf;
    std::vector<std::string> indexFiles = conf.getIndexFiles();

    std::string content;

    int status = !HttpStatus::OK;

    for (int i = 0; i < indexFiles.size() && status != HttpStatus::OK; i++)
    {
        // std::cout << status << "|" << directory + indexFiles[i] << std::endl;
        content = readFile(directory + indexFiles[i], status);
        if (status == HttpStatus::OK)
            filename = indexFiles[i];
    }
    return content;
}

// int     FileSystem::openFile(std::string filename, int &status, struct stat *state)
// {
//     struct stat info;
//     if ((stat(filename.c_str(), &info)) == -1)
//     {
//         status = HttpStatus::NotFound;
//         return ("");
//     }
//     off_t fileLength = info.st_size;
//     if ((int)((info.st_mode & S_IFDIR) == S_IFDIR))
//     {
//         status = IS_DIRECTORY;
//         return (-1);
//     }
//     int fd = open(filename.c_str(), O_RDONLY);
//     if (fd < 1)
//     {
//         status = HttpStatus::Forbidden;
//         return ("");
//     }
// }