#include "Utils.hpp"

std::string util::ltrim(const std::string &s)
{
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == std::string::npos) ? "" : s.substr(start);
}
 
std::string util::rtrim(const std::string &s)
{
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}
 
std::string util::trim(const std::string &s) {
    return rtrim(ltrim(s));
}

bool    util::is_number(std::string str)
{
	for (int i = 0; str[i] != '\0'; i++)
	{
		if (std::isdigit(str[i]) == 0)
				return (false);
	}
	return (true);
}

std::vector<std::string> util::split(const std::string& str, const std::string& delimiter) 
{
    std::vector<std::string> strings;

    std::string::size_type pos = 0;
    std::string::size_type prev = 0;
    while ((pos = str.find(delimiter, prev)) != std::string::npos)
    {
        strings.push_back(str.substr(prev, pos - prev));
        prev = pos + 1;
    }
    strings.push_back(str.substr(prev));
    return strings;
}

bool util::is_valid_method(std::string method)
{
	if (method == "GET" || method == "POST" || method == "DELETE")
		return (true);
	return (false);
}
bool util::is_valid_version(std::string ver)
{
	if (ver == "HTTP/1.1")
		return (true);
	return (false);
}
