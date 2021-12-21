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

std::string	util::ft_itos(int nb)
{
	std::ostringstream convert;   // stream used for the conversion
	convert << nb;
	return convert.str();
}

std::vector<std::string> util::split(const std::string& str, const std::string& delimiter) 
{
    std::vector<std::string> strings;

    std::string::size_type pos = 0;
    std::string::size_type prev = 0;
    while ((pos = str.find(delimiter, prev)) != std::string::npos)
    {
		if (!str.empty())
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
	std::cerr << fileName << std::endl;
	if (fileName[0] == '.')
		return "";
	size_t index = fileName.find_last_of(".");
	if(index != std::string::npos && (fileName.find_last_of("/") + 1) != index)
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

int util::getFileLength(int fd)
{
	off_t fsize;

	fsize = lseek(fd, 0, SEEK_END);
	return fsize;
}

void	util::closeAllListeners()
{
	std::cerr << clients.size() << std::endl;
	for (int i = 0; i < clients.size();i++)
		close(clients[i]);
	std::cerr << serversSockets.size() << std::endl;
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

std::string	util::getFullUrl(std::string location, std::string host)
{
	std::string fullUrl;
	fullUrl = "http://" + host + "/" + location;
	return fullUrl;
}

size_t  util::to_hex(std::string &str)
{
	std::stringstream ss;
	size_t hex;

	ss << std::hex << str;
	ss >> hex;

	return hex;
}

std::string util::DecimalToHex(int decimal_value)
{
	std::stringstream ss;
	ss<< std::hex << decimal_value;
	std::string res ( ss.str() );
	return (res);
}
size_t util::HexToDecimal(std::string &str)
{
	std::stringstream ss;
	size_t hex;

	ss << std::hex << str;
	ss >> hex;

	return hex;
}

std::string util::ft_chunked(std::vector<std::string> strings, bool flag)
{
	std::string chunkedBody;

	chunkedBody.assign("");
	for (std::vector<std::string>::iterator it = strings.begin(); it != strings.end(); it++)
	{
		chunkedBody.insert(chunkedBody.size(), DecimalToHex((*it).size()));
		chunkedBody.insert(chunkedBody.size(), "\r\n");
		chunkedBody.insert(chunkedBody.size(), *it);
		chunkedBody.insert(chunkedBody.size(), "\r\n");
	}
	if (flag)
		chunkedBody.insert(chunkedBody.size(), "0\r\n\r\n");
	return (chunkedBody);
}
//Split a string into smaller chunks cpp
/*
String lengths that are a multiple of the chunk size.
String lengths that are NOT a multiple of the chunk size.
String lengths that are smaller than the chunk size.
NULL and empty strings (throws an exception).
Chunk sizes smaller than 1 (throws an exception).
*/
std::vector<std::string> util::ChunkStrings(std::string &str, int chunkSize)
{
	std::vector<std::string> chunks;
	int i = 0;
	if (str.empty() || chunkSize < 1)
		return std::vector<std::string>();
	//div() function : Returns the integral quotient and remainder of the division of number by denom 
	div_t divResult = div(str.size(), chunkSize); // Quotient: the number multiple  / Remainder :  what rest
	int numberOfChunks = divResult.rem > 0 ? divResult.quot + 1 : divResult.quot; // here if we have a rest lets add new chunk for it
	for(i = 0; i < numberOfChunks - 1; i++)
		chunks.push_back(str.substr(i * chunkSize, chunkSize));
	int lastChunkSize = divResult.rem > 0 ? divResult.rem : chunkSize;
	chunks.push_back(str.substr(i * chunkSize, lastChunkSize));
	return (chunks);
}


std::string util::ParseChunkBody(std::string &unchunked, std::string buffer, bool &flag)
{
	std::string chunked;
	std::string hex;
	int size;
	int i = 0;

	if (!unchunked.empty())
		buffer.insert(0, unchunked);
	unchunked.assign("");
	chunked.assign("");
	hex = buffer.substr(0, buffer.size());
	size = HexToDecimal(hex);
	while (size)
	{
		std::cerr << "size: " << size << std::endl;
		i = buffer.find("\r\n", i) + 2;
		if (buffer.substr(i, size).size() >= size)
			chunked.insert(chunked.size(), buffer.substr(i, size));
		i += size + 2;
		if (i < buffer.size())
			hex = buffer.substr(i, buffer.size());
		else
			break;
		size = HexToDecimal(hex);
	}
	flag = !size;
	if (flag == 0)
		unchunked.assign(hex);
	std::cout << "****************************************" << std::endl;
	std::cout << "body|" << chunked << "|" << std::endl;
	std::cout << "rem" << unchunked << std::endl;
	std::cout << "****************************************" << std::endl;

	return (chunked);
}