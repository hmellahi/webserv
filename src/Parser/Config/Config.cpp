#include "Config.hpp"

std::map<std::string, void (Config::*)(std::vector<std::string>::iterator &)> attributes;
void Config::init_map_attr()
{
	attributes["root"] = &Config::root;
	attributes["page_error"] = &Config::page_error;
	attributes["index"] = &Config::index;
	attributes["client_max_body_size"] = &Config::client_max_body_size;
	attributes["auto_index"] = &Config::isAutoIndexOn;
	attributes["listen"] = &Config::listen;
	attributes["server_name"] = &Config::server_name;
	attributes["redirection"] = &Config::redirectionPath;
	attributes["allow_methods"] = &Config::allow_methods;
	attributes["upload_path"] = &Config::upload_path;
	attributes["cgi"] = &Config::cgi;
	attributes["location"] = &Config::locations;
}

std::map<std::string, void (Config::*)(std::vector<std::string>::iterator &)> attribute_locations;
void Config::init_map_loc()
{
	attribute_locations["root"] = &Config::root;
	attribute_locations["page_error"] = &Config::page_error;
	attribute_locations["client_max_body_size"] = &Config::client_max_body_size;
	attribute_locations["auto_index"] = &Config::isAutoIndexOn;
	attribute_locations["redirection"] = &Config::redirectionPath;
	attribute_locations["allow_methods"] = &Config::allow_methods;
	attribute_locations["upload_path"] = &Config::upload_path;
	attribute_locations["cgi"] = &Config::cgi;
	attribute_locations["index"] = &Config::index;
}

Config::Config()
{
	_isAutoIndexOn = false;
	init_map_attr();
	init_map_loc();
}
Config::Config(Config const &src)
{
	*this = src;
}
Config &Config::operator=(Config const &rhs)
{
	if (this != &rhs)
	{
		_root = rhs._root;
		_error_pages = rhs._error_pages;
		_client_max_body_size = rhs._client_max_body_size;
		_isAutoIndexOn = rhs._isAutoIndexOn;
		_index = rhs._index;
		_redirectionPath = rhs._redirectionPath;
		_allowedMethods = rhs._allowedMethods;
		_uploadPath = rhs._uploadPath;
		_cgi = rhs._cgi;
		_server_name = rhs._server_name;
		_listen = rhs._listen;
		_locations = rhs._locations;
	}
	return (*this);
}
Config::~Config()
{

}
void Config::check_server(std::vector<std::string>::iterator &it)
{
	if (*it != "{")
		throw std::runtime_error("Error: Missing a bracket in server");	
	while (*(++it) != "}")
	{
		if (attributes[*it])
			(this->*attributes[*it])(++it);
		else
			throw std::runtime_error("Error: Something Wrong in the config File");
	}
}

void Config::root(std::vector<std::string>::iterator &it)
{
	_root = *it;
	it++;
	if (*it != ";")
		throw std::runtime_error("Error: Bad Root.");
}

void Config::page_error(std::vector<std::string>::iterator &it)
{
	int code;
	if (util::is_number(*it))
		code = atoi((*it).c_str());
	else
		throw std::runtime_error("Invalid Status Code: " + *it);
	if (code < 400 || code > 511)
		throw std::runtime_error("Invalid Status Code: " + *it);
	it++;
	std::string path_error;
	path_error = *it;
	if (path_error.empty())
		throw std::runtime_error("Invalid Path Page Error");
	it++;
	_error_pages.insert(std::pair<int, std::string>(code, path_error));
}

void Config::client_max_body_size(std::vector<std::string>::iterator &it)
{
	if (util::is_number(*it))
		_client_max_body_size = atoi((*it).c_str());
	else
		throw std::runtime_error("Error: Invalid client_max_body_size");
	it++;
	if (*it != ";")
		throw (std::runtime_error("Error: Something wrong with client_max_body_size"));
}

void Config::isAutoIndexOn(std::vector<std::string>::iterator &it)
{
	if (*it == "on")
		_isAutoIndexOn = true;
	else if (*it == "off")
		_isAutoIndexOn = false;
	else
		throw (std::runtime_error("Error: Invalid auto_index"));
	it++;
	if (*it != ";")
		throw std::runtime_error("Error: Something wrong with auto_index ");
}

void Config::redirectionPath(std::vector<std::string>::iterator &it)
{
	int code;
	if (util::is_number(*it))
		code = atoi((*it).c_str());
	else
		throw std::runtime_error("Invalid Status Code: " + *it);
	it++;
	std::string path;
	path = *it;
	if (path.empty())
		throw std::runtime_error("Invalid Path Redirection");
	it++;
	_redirectionPath.first = code;
	_redirectionPath.second = path;
}

void Config::allow_methods(std::vector<std::string>::iterator &it)
{
	while (*it != ";")
	{
		if (util::is_valid_method(*it))
			_allowedMethods.push_back(*it);
		else
			throw std::runtime_error("Error: Invalid Method: "+ *it);
		it++;
	}
}

void Config::upload_path(std::vector<std::string>::iterator &it)
{
	_uploadPath = *it;
	it++;
	if (*it != ";")
		throw std::runtime_error("Error: Something wrong with upload Path.");
}

void Config::cgi(std::vector<std::string>::iterator &it)
{
	std::string first = *it;
	it++;
	std::string second = *it;
	it++;
	if (first.empty() || second.empty() || *it != ";")
		throw std::runtime_error("Error : something wrong with cgi");
	_cgi.insert(std::pair<std::string, std::string>(first, second));
}

void Config::server_name(std::vector<std::string>::iterator &it)
{
	while (*it != ";")
	{
		_server_name.push_back(*it);
		it++;
	}
}

void Config::listen(std::vector<std::string>::iterator &it)
{
	u_int32_t port = 8000;
	int count = 0;
	const char *str;
	while (*it != ";")
	{
		str = (*it).c_str();
		if (util::is_number(*it))
		{
			port = atoi(str);
			if (port < 0 || port > 65535)
				throw std::runtime_error("Invalid Port: " + *it + ". Sould be between 0 and 65535");
		}
		else
			throw std::runtime_error("Invalid Port: " + *it);
		_listen.push_back(port);
		it++;
		count++;
	}
	if (count != 1)
		throw std::runtime_error("Error: Bad in port .");
}

void Config::locations(std::vector<std::string>::iterator &it)
{
	Config loc;

	loc = *this;
	std::string path = *it;
	if (path.empty())
		throw std::runtime_error("Error: Something wrong with Location");
	it++;
	if (*it != "{")
		throw std::runtime_error("Error: Missing a bracket in location");
	while (*(++it) != "}")
	{
		if (attribute_locations[*it])
			(loc.*attribute_locations[*it])(++it);
		else
			throw std::runtime_error("Error: Something Wrong in the config File");
	}
	_locations[path] = loc;
}

void Config::index(std::vector<std::string>::iterator &it)
{
	while (*it != ";")
	{
		_index.push_back(*it);
		it++;
	}
}

std::string Config::getRoot(void) const
{
	return (_root);
}

std::map<std::string, Config> Config::getLocation(void) const
{
	return (_locations);
}

std::map<int, std::string> Config::get_error_pages(void) const
{
	return (_error_pages);
}

int Config::get_client_max_body_size(void) const
{
	return (_client_max_body_size);
}

std::vector<std::string> Config::getIndex(void) const
{
	return (_index);
}

std::pair<int, std::string> Config::get_redirectionPath(void) const
{
	return (_redirectionPath);
}

std::vector<std::string> Config::get_allowedMethods(void) const
{
	return (_allowedMethods);
}

std::string Config::getUploadPath(void) const
{
	return (_uploadPath);
}

std::map<std::string, std::string> Config::getCGI(void) const
{
	return (_cgi);
}

std::vector<std::string> Config::getServerName(void) const
{
	return (_server_name);
}

std::vector<u_int32_t> Config::getPorts(void) const
{
	return (_listen);
}

bool Config::get_isAutoIndexOn(void) const
{
	return (_isAutoIndexOn);
}

void	Config::setIndex(std::vector<std::string> indexFiles)
{
	_index = indexFiles;
}

void	Config::setAllowedMethods(std::vector<std::string> allowedMethods)
{
	_allowedMethods = allowedMethods;
}

void	Config::setRoot(std::string root)
{
	_root = root;
}

// template<typename T>
// T	Request::getAttribute(std::string attributeName, std::string location) const
// {
// 	return (_attributes[attributeName]);
// }

		// template <typename T>
		// T	getAttribute(std::string attributeName, std::string location) const;

// Config Config::getConfig(std::string path) const
// {
// 	if (location != _locations.end())
// 		return location->second;
// 	return (*this);
// }

// std::string Config::getRoot(std::string path) const
// {
// 	return (getConfig(path).getRoot());
// }

// std::string Config::getUploadPath(std::string path) const
// {
// 	return (getConfig(path).getRoot());
// }

// int Config::getClientMaxBodySize(std::string path) const
// {
// 	return (getConfig(path).get_client_max_body_size());
// }

// std::vector<std::string> Config::getAllowedMethods(std::string path) const
// {
// 	return (getConfig(path).get_allowedMethods());
// }

// std::pair<int, std::string> Config::getRedirectionPath(std::string path) const
// {
// 	return (getConfig(path).get_redirectionPath());
// }

// std::vector<std::string> Config::getIndex(std::string path) const
// {
// 	return (getConfig(path).getIndex());
// }

// bool Config::isAutoIndexOn(std::string path) const
// {
// 	return (getConfig(path).get_isAutoIndexOn());
// }