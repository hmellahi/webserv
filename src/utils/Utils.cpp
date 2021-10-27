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

// get current date and time
//output ex : Sun, 17 Oct 2021 23:38:35 GMT
std::string util::getCurrentDate()
{
	// current date/time based on current system
	char outstr[200] = {0};
	time_t t;
	struct tm *tmp;
	const char* fmt = "%a, %d %b %Y %T GMT"; // https://www.cplusplus.com/reference/ctime/strftime/

	t = time(NULL);
	tmp = gmtime(&t);
	if (tmp == NULL) {
		perror("gmtime error");
		util::ft_exit(EXIT_FAILURE);
	}

	if (strftime(outstr, sizeof(outstr), fmt, tmp) == 0) { 
		fprintf(stderr, "strftime returned 0");
		util::ft_exit(EXIT_FAILURE); 
	} 
   return (outstr);
}

std::string util::GetFileExtension(const std::string& fileName)
{
	if(fileName.find_last_of(".") != std::string::npos)
		return fileName.substr(fileName.find_last_of(".") + 1);
	return "";
}

int util::getFileLength(const std::string& filename)
{
	struct stat info;

    stat(filename.c_str(), &info);
    off_t fileLength = info.st_size;

	return fileLength;
}

void	util::closeAllListeners()
{
	for (int i = 0; i < serversSockets.size();i++)
		close(serversSockets[i]);
}

void	util::signal_handler(int signal)
{
	closeAllListeners();
}

void	util::ft_exit(int status)
{
	closeAllListeners();
	exit(status);
}
