
#include "run_server.hpp"
#include <iostream>
#include <stdlib.h>
#include <configParser.hpp>

int	main(int ac, char **av)
{
	(void)av;
	if (ac != 2)
	{
		std::cerr << "Error: args" << std::endl;
		return (EXIT_FAILURE);
	}
	configParser	server;
	try {
		server.parse(av[1]);
	}
	catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}
	int exit_sts = run(server.getServers());
	return (exit_sts);
}
