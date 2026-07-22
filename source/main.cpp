
#include "run_server.hpp"
#include "utils_logs.hpp"
#include <cerrno>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <stdlib.h>
#include <configParser.hpp>

int	main(int ac, char **av)
{
	(void)av;
	if (ac != 2)
	{
		std::cerr << "Error: usage: " << av[0] << " <config.conf>" << std::endl;
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
	int	exit_sts = 0;
	try
	{
		exit_sts = run(server.getServers());
	}
	catch (std::exception& e)
	{
		exit_sts = errno;
	}
	return (exit_sts);
}
