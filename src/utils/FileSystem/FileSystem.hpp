#pragma once
#include "macros.hpp"
#include <vector>

namespace FileSystem
{
    std::string  readFile(std::string filename, int &status);
    std::string  openFile(std::string filename, int &status);
    std::string  getIndexFile(const std::string & path, const std::vector<std::string> &indexFiles);
    int          getFileStatus(std::string filename);
};