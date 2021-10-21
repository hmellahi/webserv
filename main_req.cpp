#include "parser/Request.hpp"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

int main ()
{
	try
	{
		string line;
		ifstream myfile ("request.txt");

		std::string buffer;
		if (myfile.is_open())
		{
			while ( getline (myfile,line) )
			{
				buffer += line + "\n";
			}
			myfile.close();
		}
		else
			cout << "Unable to open file"; 
		/*------------------------------------*/
		// std::cout << buffer;
		Request request(buffer);
		
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return 0;
}