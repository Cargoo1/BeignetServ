#include <serverConfig.hpp>

serverConfig::serverConfig() {}
serverConfig::serverConfig(const serverConfig &rhs) : _port(rhs._port), _serverName(rhs._serverName), _index(rhs._index), _errorPages(rhs._errorPages), _clientMaxBodySize(rhs._clientMaxBodySize), _locations(rhs._locations) {}
serverConfig::~serverConfig() {}

serverConfig &serverConfig::operator=(const serverConfig &rhs) {
		if (this != &rhs) {
		this->_port = rhs._port;
		this->_serverName = rhs._serverName;
		this->_errorPages = rhs._errorPages;
		this->_clientMaxBodySize = rhs._clientMaxBodySize;
		this->_locations = rhs._locations;
	}
	return (*this);
}

