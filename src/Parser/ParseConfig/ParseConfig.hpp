#ifndef PARSE_CONFIG_HPP
#define PARSE_CONFIG_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <exception>
#include "../utils/Utils.hpp"
#include <algorithm>
#include <iterator>
#include <vector>
#include <stack>
#include "Config.hpp"

class ParseConfig
{
	private:
		const char* _file;
		std::vector<std::string> tokens;
		std::vector<Config> servers;
		std::stack<bool> brackets;
	public:
		ParseConfig();
		ParseConfig(const char* file);
		ParseConfig(ParseConfig const &src);
		ParseConfig &operator=(ParseConfig const &rhs);
		virtual ~ParseConfig();
		void read_file();
		void parse_file(std::ifstream &my_file);
		void cutting_line(std::string line);
		void fill_attributes();
		std::vector<Config> getServers();
		class UnableOpenFileException: public std::exception
		{
			virtual const char *what() const throw()
			{
				return ("invalid File.");
			}
		};		
};

#endif