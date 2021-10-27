#include "Indexing.hpp"

Indexing::Indexing(std::string root, std::string url): _root(root), _url(url)
{
	_body = "<html>\n<head>\n<title>Index of " + _url + "</title></head>\n<body>\n<h1>Index of " + url + "</h1>\n<hr>\n";
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
		_root = rhs._root;
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
	std::string name;
	std::string path = _root + _url;
	dr = opendir(path.c_str());
	// dr = opendir("tests/staticWebsite/");
	if (dr)
	{
		while ((file = readdir(dr)) != NULL)
		{
			name = file->d_name;
			if (name == "..")
				name = "../";
			else if (name == ".")
				continue ;
			_files.push_back(name);
		}
		closedir(dr);
		generateTable();
	}
}

void Indexing::generateTable()
{
	std::vector<std::string>::iterator it;
	std::string path;
	std::string size;
	for (it = _files.begin(); it != _files.end(); it++)
	{
		path = _url + *it;
		size = SizeFile(path);
		if (*it == "../")
			_body = _body + "<tr>\n<td style=\"width: 55%;\"><pre><a href='" + *it + "'>" + *it + "</a></pre></td>\n</tr>";
		else
			_body = _body + "<tr>\n<td style=\"width: 55%;\"><a href='" + *it + "'>" + *it + "</a></td>\n<td>" + last_modified(path) + "</td>\n<td>" + size + "</td>\n</tr>\n";
	}
}

std::string Indexing::last_modified(std::string path)
{
	struct tm* clock;
	struct stat buff;
	char buffer[100];

	path = _root + path;
	std::cout << "path: " << path << std::endl;
	if (stat(path.c_str(), &buff) == 0)
	{
		clock = gmtime(&(buff.st_mtime));
		strftime(buffer, 100, "%d-%b-%Y %H:%M", clock);
		return (buffer);
	}
	return ("");
}
std::string Indexing::SizeFile(std::string path)
{
	struct stat buff;
	std::string size;
	std::stringstream ss;

	path = _root + path;
	std::cout << "path: " << path << std::endl;
	if (stat(path.c_str(), &buff) == 0)
	{
		ss << buff.st_size;    
		ss >> size;  
		return (size);
	}
	return ("");
}

std::string Indexing::getBody(void) const
{
	return (_body);
}