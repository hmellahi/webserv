/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamza <hamza@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/16 23:53:46 by hamza             #+#    #+#             */
/*   Updated: 2021/10/17 06:42:39 by hamza            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "macros.hpp"

#pragma once
class Config
{
private:
	bool						_isAutoIndexOn;
	std::string				_rootPath;
	std::string				_uploadRoute;
	std::string				_serverName;
	int        				_RequestbodySizeLimit;
	std::vector<std::string> _indexFiles;
	std::map<int, std::string> _customErrorPages;


public:

	Config()
	{
		_isAutoIndexOn = false;
		
		_indexFiles.push_back("index.html");
		_indexFiles.push_back("index.php"); 
	
		_customErrorPages[HttpStatus::NotFound] = "src/Config/CustomErrorPages/404.html";
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
		return _customErrorPages;
	}
};

