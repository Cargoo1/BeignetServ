#pragma once

#include <map>
#include <string>
#include <vector>

#include <configException.hpp>
#include <locationConfig.hpp>
#include <serverConfig.hpp>


/*
ConfigParser        ← orchestre tout le parsing
        │
        ▼
ServerConfig        ← représente un bloc server
        │
        ▼
LocationConfig      ← représente un bloc location
        │
RouteConfig         ← (optionnel) gestion des redirections
*/

class configParser {
	public:
		configParser();
		configParser(const configParser &rhs);
		~configParser();

		configParser					&operator=(const configParser &rhs);
		const std::vector<serverConfig>	&getServers(void);

		void parse(std::string file_conf);

	private:
		std::string					_filepath;
		std::string					_content;
		std::vector<std::string>	_tokens;
		size_t						_pos;
		std::vector<serverConfig>	_servers;

		void _readFile(std::ifstream file_conf);
		void _tokenize();
		void _parseServer();
		void _parseLocation();
};
