#ifndef INDEXING_HPP
#define INDEXING_HPP

#include <iostream>
#include <dirent.h>
#include <time.h>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>

class Indexing
{
	public:
		Indexing(std::string url);
		Indexing(Indexing const &src);
		Indexing &operator=(Indexing const &rhs);
		~Indexing();
		void createTable();
		std::string last_modified(std::string path);
		int SizeFile(std::string path);
		std::string getBody(void) const;
	private:
		std::string _url;
		std::string _body;
};
#endif