#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>

const std::string WHITESPACE = " \n\r\t\f\v";

namespace util
{
    std::string ltrim(const std::string &s);
    std::string rtrim(const std::string &s);
    std::string trim(const std::string &s);
    bool        is_number(std::string str);
}
#endif