#include "Webserv.hpp"

void read_config(ParseConfig &ParseConfig)
{
	std::vector<Config> servers = ParseConfig.getServers();
	std::vector<Config>::iterator it;
	std::map<std::string, Config>::iterator it_loc;
	for (it = servers.begin(); it != servers.end(); it++)
	{
		std::cout << "-------------  Server Information    -------------\n";
		std::cout <<"Root:" << it->getRoot() << std::endl;
		std::cout <<"Server Name: ";
		std::vector<std::string> server_names = it->get_server_name();
		for (std::vector<std::string>::iterator sn = server_names.begin(); sn != server_names.end(); sn++)
		{
			std::cout  << *sn << " ";
		}
		std::cout << "\n";
		std::cout <<"Max Body:" << it->get_client_max_body_size() << std::endl;
		std::cout <<"Auto Index:" << it->get_isAutoIndexOn() << std::endl;
		std::map<std::string, Config> loc = it->getLocation();
		for (it_loc = loc.begin(); it_loc != loc.end(); it_loc++)
		{
			std::cout << "In locations: Root: " << it_loc->second.getRoot() << " || upload_path: " << it_loc->second.get_uploadPath() << std::endl;
			std::cout <<"-----> Server Name: ";
			std::vector<std::string> server_names = it_loc->second.get_server_name();
			for (std::vector<std::string>::iterator sn = server_names.begin(); sn != server_names.end(); sn++)
			{
				std::cout  << *sn << " ";
			}
			std::cout << "\n";
		}
		std::cout << "------------- End Server Information -------------\n\n";

	}
}
// 1 - first thing to do is parsing the config file
int main(int argc, char *argv[])
{
	try
	{
		if (argc <= 2)
		{
			const char * file;

			if (argv[1])
				file = argv[1];
			else
				file = DEFAULTFILE;
			ParseConfig ParseConfig(file);
			read_config(ParseConfig);
		}
		else
		{
			std::cerr << "Error: Wrong Number of Argument ." << std::endl;
			exit(EXIT_FAILURE);
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return (0);
}