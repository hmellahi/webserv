#include "ParseConfig.hpp"
#include "../../utils/FileSystem/FileSystem.hpp"

ParseConfig::ParseConfig()
{
}

ParseConfig::ParseConfig(const char* file): _file(file)
{
	read_file();
}

ParseConfig::ParseConfig(ParseConfig const &src)
{
	*this = src;
}

ParseConfig &ParseConfig::operator=(ParseConfig const &rhs)
{
	if (this != &rhs)
	{
		this->_file = rhs._file;
		this->servers = rhs.servers;
	}
	return (*this);
}

ParseConfig::~ParseConfig()
{
}

// open the file 
void ParseConfig::read_file()
{
	std::ifstream my_file(_file);

	if (my_file.is_open() && FileSystem::getFileStatus(_file) != IS_DIRECTORY)
	{
		parse_file(my_file);
		my_file.close();
	}
	else
		throw (UnableOpenFileException());
}

// remove whitespaces and comments and push the line in vector 
void ParseConfig::parse_file(std::ifstream &my_file)
{
	std::string line;
	while (getline(my_file, line))
	{
		line = util::trim(line);
		std::size_t found = line.find("#");
		if (found != std::string::npos)
			line.erase(found);
		if (!line.empty())
			cutting_line(line);
	}
	fill_attributes();
}

// cut the line with whitespace and check if the brackets work good 
// add a point-virgule for know the end of each line
void ParseConfig::cutting_line(std::string line)
{
	std::string token;
	size_t first = 0, last = 0;
	
	while ((first = line.find_first_not_of(" \t", last)) != std::string::npos)
	{
		last = line.find_first_of(" \t", first);
		token = line.substr(first, last - first);
		if (token == "{")
			brackets.push(true);
		else if (token == "}")
		{
			if (brackets.empty())
				throw std::runtime_error("Error: Missing a bracket in server");	
			brackets.pop();
		}
		tokens.push_back(token);
	}
	if (token != "{" && token != "}")
		tokens.push_back(";");
}

// here lets search for serve and make a object for it and fill the config
// if we not found the server we throw error
// push the object to servers vector
void ParseConfig::fill_attributes()
{
	std::vector<std::string>:: iterator it;
	for (it = tokens.begin(); it != tokens.end(); it++)
	{
		if (*it == "server")
		{
			Config server;
			it++;
			server.check_server(it);
			servers.push_back(server);
		}
		else
			throw std::runtime_error("Error: Not found server Block!");
	}	
}

std::vector<Config> ParseConfig::getServers()
{
	return (servers);
}