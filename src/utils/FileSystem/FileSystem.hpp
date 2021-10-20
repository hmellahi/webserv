#pragma once
#include "macros.hpp"
#include <vector>

class FileSystem
{
public:
    static std::string  readFile(std::string filename, int &status);
    static std::string  openFile(std::string filename, int &status);
    static std::string  getIndexFileContent(std::string directory, std::string & filename);
};