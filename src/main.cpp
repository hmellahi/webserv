#include "macros.hpp"
#include "ParseConfig.hpp"
#include "Server.hpp"
#include <csignal>

void	handle(int a){
	std::cerr << "quited with signal : " << a << std::endl;
	// util::signal_handler(a);
	// system("leaks webserv");
	util::ft_exit(0);
}
int main(int ac , char *av[])
{
	std::signal(SIGINT, handle);
	std::signal(SIGKILL, handle);
	std::signal(SIGBUS, handle);

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
		std::cerr << "error [" << e.what() << "]: " << '\n';
		util::ft_exit(EXIT_FAILURE);
	}
	// system("leaks webserv");
	return 0;
}
