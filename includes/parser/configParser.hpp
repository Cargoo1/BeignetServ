#pragma once

#include <map>
#include <string>
#include <vector>

#include <exception/configException.hpp>
#include <configClass/locationConfig.hpp>
#include <configClass/serverConfig.hpp>
#include <parser/Token.hpp>


class configParser {
	public:
		configParser();
		configParser(const configParser &rhs);
		~configParser();

		configParser					&operator=(const configParser &rhs);
		const std::vector<serverConfig>	&getServers(void);

		void parse(const std::string &arg);

	private:
		std::string					_content;
		std::vector<Token>			_tokens;
		std::size_t						_pos;
		std::vector<serverConfig>	_servers;

		void _readFile(const std::string &arg);
		void _tokenize(void);
		void _parseServer(serverConfig &toParse);
		void _parseDirective(serverConfig &toParse);
		void _parseLocation(locationConfig &locTo_add);
		void _parseLocationDir(locationConfig &locTo_add);
		void _expect(const std::string &to_compare);
		Token _peek(void);
		Token _consume(void);
		void _validateAll(void);
};
