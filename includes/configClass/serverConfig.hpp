#pragma once

#include "locationConfig.hpp"

#include <map>
#include <string>
#include <vector>

enum DIR { LISTEN, SERVER_NAME, ERROR_PAGE, INDEX, CLIENT_MAX_BODY, NONE};

class serverConfig {
	friend class configParser;
	public:
		serverConfig();
		serverConfig(const serverConfig &rhs);
		~serverConfig();

		serverConfig &operator=(const serverConfig &rhs);

	private:
		std::string					_port;
		std::string					_serverName;
		std::string					_index;
		std::map<int, std::string>	_errorPages;
		std::size_t					_clientMaxBodySize;
		std::vector<locationConfig>	_locations;
};