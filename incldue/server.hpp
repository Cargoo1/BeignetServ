#pragma once

#include <map>
#include <string>

struct	{
	std::string		name;
	unsigned int	port;
}

class server {
	public:
		server();
		server(const server &rhs);
		~server();

		server &operator=(const server &rhs);

	private:
		std::map<unsigned int, std::string>	_locs;
};