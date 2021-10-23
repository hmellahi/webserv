#include "macros.hpp"
#include "ParseConfig.hpp"
#include "Server.hpp"

// note only files are working rn
// ex : http://localhost:port/tests/staticWebsite/index.html

int main(int ac , char *av[])
{
	try
	{
		if (ac <= 2)
		{
			const char *file = av[1] ? av[1] : DEFAULT_CONFIG_PATH;
			// parse the config file
			ParseConfig GlobalConfig(file);
			// then setup servers
			Server::setup(GlobalConfig);
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
	return 0;
}
