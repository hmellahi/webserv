#ifndef INDEXING_HPP
#define INDEXING_HPP

#include <iostream>
#include <dirent.h>
#include <time.h>
#include <string>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include<sstream>  

class Indexing
{
	public:
		Indexing(std::string url);
		Indexing(Indexing const &src);
		Indexing &operator=(Indexing const &rhs);
		~Indexing();
		void createTable();
		void generateTable();
		std::string last_modified(std::string path);
		std::string SizeFile(std::string path);
		std::string getBody(void) const;
	private:
		std::string _url;
		std::string _body;
		std::vector<std::string> _files;
};
#endif