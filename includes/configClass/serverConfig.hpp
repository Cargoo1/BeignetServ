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

		std::string &getRoot();
		const std::string &getRoot() const;

		bool const& hasIndex() const;
	
		std::string &getIndex();
		const std::string &getIndex() const;

		std::map<int, std::string> &errorPages();
		const std::map<int, std::string> &errorPages() const;

		unsigned long long &getCMBS();
		const unsigned long long &getCMBS() const;

	private:
		std::string					_listen;
		std::string					_serverName;
		std::string					_root;
		std::string					_index;
		bool						_hasIndex;
		std::map<int, std::string>	_errorPages;
		unsigned long long			_clientMaxBodySize;
		std::vector<locationConfig>	_locations;
};
