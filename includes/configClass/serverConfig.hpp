#pragma once

#include <locationConfig.hpp>

#include <map>
#include <string>
#include <vector>

class serverConfig {
	public:
		serverConfig();
		serverConfig(const serverConfig &rhs);
		~serverConfig();

		serverConfig &operator=(const serverConfig &rhs);

	private:
		std::string					_host;
		std::string					_port;
		std::string					_serverName;
		std::map<int, std::string>	_errorPages;
		size_t						_clientMaxBodySize;
		std::vector<locationConfig>	_locations;
};