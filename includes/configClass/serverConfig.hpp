#pragma once

#include "locationConfig.hpp"

#include <map>
#include <string>
#include <vector>

enum DIR { LISTEN, SERVER_NAME, ROOT, ERROR_PAGE, INDEX, CLIENT_MAX_BODY, NONE};

class serverConfig {
	friend class configParser;
	public:
		serverConfig();
		serverConfig(const serverConfig &rhs);
		~serverConfig();

		serverConfig &operator=(const serverConfig &rhs);

		std::string listen();
		const std::string listen() const;

		std::string serverName();
		const std::string serverName() const;

		std::vector<locationConfig> locationBloc();
		const std::vector<locationConfig> locationBloc() const;

		std::string root();
		const std::string root() const;

	public:
		std::string					_listen;
		std::string					_serverName;
		std::string					_root;
		std::string					_index;
		std::map<int, std::string>	_errorPages;
		std::size_t					_clientMaxBodySize;
		std::vector<locationConfig>	_locations;
};
