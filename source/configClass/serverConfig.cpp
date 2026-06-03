#include <serverConfig.hpp>

serverConfig::serverConfig() {}
serverConfig::serverConfig(const serverConfig &rhs) : _listen(rhs._listen), _serverName(rhs._serverName), _root(rhs._root), _index(rhs._index), _errorPages(rhs._errorPages), _clientMaxBodySize(rhs._clientMaxBodySize), _locations(rhs._locations) {}
serverConfig::~serverConfig() {}

serverConfig &serverConfig::operator=(const serverConfig &rhs) {
	if (this != &rhs) {
		this->_listen = rhs._listen;
		this->_serverName = rhs._serverName;
		this->_root = rhs._root;
		this->_errorPages = rhs._errorPages;
		this->_clientMaxBodySize = rhs._clientMaxBodySize;
		this->_locations = rhs._locations;
	}
	return (*this);
}

std::string serverConfig::listen() {
	return (this->_listen);
}

const std::string serverConfig::listen() const {
	return (this->_listen);
}

std::string serverConfig::serverName() {
	return (this->_serverName);
}

const std::string serverConfig::serverName() const {
	return (this->_serverName);
}

