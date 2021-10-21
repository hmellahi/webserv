#include "Indexing.hpp"
Indexing::Indexing(std::string url): _url(url)
{
	_body = "<html>\n<head>\n<title>Index of " + url + "</title></head>\n<body>\n<h1>Index of " + url + "</h1>\n<hr>\n";
	_body += "<table style=\"width: 40%;\">\n";
	createTable();
	_body += "</table>\n<hr>\n</body>\n</html>";
}

Indexing::Indexing(Indexing const &src)
{
	*this = src;
}

Indexing & Indexing::operator=(Indexing const &rhs)
{
	if (this != &rhs)
	{
		_url = rhs._url;
		_body = rhs._body;
	}
	return (*this);
}

Indexing::~Indexing()
{
	
}

void Indexing::createTable()
{
	DIR *dr;
	struct dirent *file;
	std::string path;
	dr = opendir(_url.c_str());
	if (dr)
	{
		while ((file = readdir(dr)) != NULL)
		{
			path = (_url[_url.size() - 1] == '/') ? _url + file->d_name : _url + "/" + file->d_name;
			_body += "<tr>\n";
			_body += "<td style=\"width: 55%;\"><a href='";
			_body += path;
			_body += "'>";
			_body += file->d_name;
			_body += "</a></td>\n<td>";
			_body += last_modified(path);
			_body += "</td>\n";
			_body += "<td>";
			_body += "0";
			_body += "</td>\n";
			_body += "</tr>\n";
		}
		closedir(dr);
	}
}

std::string Indexing::last_modified(std::string path)
{
	struct stat buff;
	time_t rawtime;
  	struct tm * timeinfo;
	char buffer[100];

	if (stat(path.c_str(), &buff) == 0)
	{
		time (&rawtime);
  		timeinfo = localtime (&rawtime);
		strftime(buffer, 100, "%d-%b-%Y %H:%M", timeinfo);
		return (buffer);
	}
	return ("");
}

int  Indexing::SizeFile(std::string path)
{
	struct stat buff;
    
	if (stat(path.c_str(), &buff) == 0)
	{
		std::cout << buff.st_size << std::endl;
		return buff.st_size;
	}
	return (0);
}

std::string Indexing::getBody(void) const
{
	return (_body);
}