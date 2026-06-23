#pragma once

#include "locationConfig.hpp"

#include <map>
#include <string>
#include <vector>

enum DIRC { LISTEN, SERVER_NAME, ROOT, ERROR_PAGE, INDEX, CLIENT_MAX_BODY, NONE};

class serverConfig {
	friend class configParser;
	public:
		serverConfig();
		serverConfig(const serverConfig &rhs);
		~serverConfig();

		serverConfig &operator=(const serverConfig &rhs);

		std::string &listen();
		const std::string &listen() const;

		std::string &serverName();
		const std::string &serverName() const;

		std::vector<locationConfig> &locations();
		const std::vector<locationConfig> &locations() const;

		std::string &root();
		const std::string &root() const;

		std::map<int, std::string> &errorPages();
		const std::map<int, std::string> &errorPages() const;

	std::size_t &getCMBS();
	const std::size_t &getCMBS() const;

	private:
		std::string					_listen;
		std::string					_serverName;
		std::string					_root;
		std::string					_index;
		std::map<int, std::string>	_errorPages;
		std::size_t					_clientMaxBodySize;
		std::vector<locationConfig>	_locations;
};
