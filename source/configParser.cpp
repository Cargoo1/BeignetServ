#include <configParser.hpp>
#include <cstring>
#include <fstream>
#include <iostream>
#include <stdexcept>

configParser::configParser() : _filepath(0), _content(0), _tokens(0), _pos(0), _servers(0) {}

configParser::configParser(const configParser &rhs) : _filepath(rhs._filepath), _content(rhs._content), _tokens(rhs._tokens), _pos(rhs._pos), _servers(rhs._servers)  {}

configParser::~configParser() {}

configParser &configParser::operator=(const configParser &rhs) {
	if (this != &rhs) {
		this->_filepath = rhs._filepath;
		this->_content = rhs._content;
		this->_tokens = rhs._tokens;
		this->_pos = rhs._pos;
		this->_servers = rhs._servers;
	}
	return (*this);
}

const std::vector<serverConfig> &configParser::getServers(void) {
	return (this->_servers);
}

void configParser::parse(std::string arg) {
	if (arg.empty())
		throw std::invalid_argument(arg);
	std::ifstream config_file(arg.c_str());
	if (!config_file)
		throw ;
	_readFile(config_file);
}

void _readFile(std::ifstream file_conf) {
	std::string line;

	getline(file_conf, line);
	while (getline(file_conf, line)) {
	}
}

void _tokenize() {

}

void _parseServer() {

}

void _parseLocation() {

}
