#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <algorithm>
#include <vector>
#include "Socket.hpp"
#include "macros.hpp"

const std::string WHITESPACE = " \n\r\t\f\v";

namespace util
{
    std::string ltrim(const std::string &s);
    std::string rtrim(const std::string &s);
    std::string trim(const std::string &s);
    bool        is_number(std::string str);
    bool is_valid_method(std::string method);
    bool is_valid_version(std::string ver);
    std::vector<std::string> split(const std::string& str, const std::string& delimiter);
    
    std::string     getCurrentDate();
    std::string     GetFileExtension(const std::string& fileName);
    int             getFileLength(const std::string& fileName);
    int             getFileLength(int fd);
    void            closeAllListeners();
    void            signal_handler(int signal);
    void            ft_exit(int status);
    std::string     getFullUrl(std::string location, std::string host);
    size_t          to_hex(std::string &str);
    size_t          HexToDecimal(std::string &str);
    std::string     ft_itos(int nb);
    std::string     DecimalToHex(int decimal_value);
    std::string     ParseChunkBody(std::string &unchunked, std::string &buffer, bool &flag);
    std::string     ft_chunked(std::vector<std::string> strings, bool flag);
    std::vector<std::string>    ChunkStrings(std::string &str, int chunkSize);

}
#endif