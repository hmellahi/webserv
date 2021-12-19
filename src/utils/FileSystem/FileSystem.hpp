#pragma once
#include "macros.hpp"
#define READ 0
#define WRITE 1
#include <vector>
# include "../HTTP/Request/Request.hpp"
namespace FileSystem
{
    std::string  readFile(std::string filename, int &status);
    std::string  openFile(std::string filename, int &status);
    std::string  getIndexFile(const std::string & path, const std::vector<std::string> &indexFiles);
    int          getFileStatus(std::string filename);
    void         uploadFile(std::string uploadLocation, std::vector<char> content);
    void         uploadChunkedFile(std::string uploadLocation, Request req);
    bool         isReadyFD(int fd, int mode);
};