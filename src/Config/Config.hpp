/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmellahi <hmellahi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/16 23:53:46 by hamza             #+#    #+#             */
/*   Updated: 2021/10/17 20:15:25 by hmellahi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "macros.hpp"

#pragma once
class Config
{
private:
	// all Contexts
	std::string					_root;
	std::map<int, std::string>	_error_pages;
	int							_client_max_body_size;
	bool						_isAutoIndexOn;
	std::pair<int, std::string> _redirection;  // return 301 /test => localhost/test
	std::vector<int>			_allowedMethods;
	std::string					_uploadPath;
	std::map<std::string, std::string>	_cgi;
	

	// Server Context only
	std::vector<std::string>			_server_name;
	std::map<std::string, Config>		_locations;

	
	std::string				_rootPath;
	std::string				_uploadRoute;
	std::string				_serverName;
	int        				_RequestbodySizeLimit;
	std::vector<std::string> _indexFiles;
	std::vector<std::string> _index;
	// std::map<std::string, Config> _locations;
	// std::vector<Method> _methods;
	bool _autoindex;


public:
	std::vector<int>				_ports;

	Config()
	{
		_isAutoIndexOn = false;
		
		_indexFiles.push_back("index.html");
		_indexFiles.push_back("index.php"); 
	
		_error_pages[HttpStatus::NotFound] = "src/Config/CustomErrorPages/404.html";
		// _customErrorPages[HttpStatus::Forbidden] = "CustomErrorPages/402.html";

		_rootPath = "/home/hamza/Desktop/webserv/tests";

		_serverName = "Weeeb Serv";
		_RequestbodySizeLimit = 200;
	}
	
	 bool isAutoIndexOn()
	{
		return _isAutoIndexOn;
	}

	std::vector<std::string> getIndexFiles()
	{
		return _indexFiles;
	}

	std::map<int, std::string> getCustomErrorPages()
	{
		return _error_pages;
	}
};

