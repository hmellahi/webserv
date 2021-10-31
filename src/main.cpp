#include "macros.hpp"
#include "ParseConfig.hpp"
#include "Server.hpp"
#include <csignal>

int main(int ac , char *av[])
{
	std::signal(SIGINT, util::signal_handler);

	const char *file;
	if (ac <= 2)
	{
		file = av[1] ? av[1] : DEFAULT_CONFIG_PATH;
	}
	else
	{
		std::cerr << "Error: Wrong Number of Argument ." << std::endl;
		util::ft_exit(EXIT_FAILURE);
	}

	try
	{
		// parse the config file
		ParseConfig GlobalConfig(file);
		// then setup servers
		Server::setup(GlobalConfig);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		util::ft_exit(EXIT_FAILURE);
	}

	return 0;
}
