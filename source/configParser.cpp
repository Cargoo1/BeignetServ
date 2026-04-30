#include <configParser.hpp>
#include <cstring>
#include <fstream>
#include <iostream>
#include <configException.hpp>

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

void configParser::parse(const std::string &arg) {
	if (arg.empty())
		throw configException("Error: empty arg ", 0, arg, errno);
	std::ifstream config_file(arg.c_str());
	if (!config_file.is_open())
		throw configException("Error: cannot open config file", 0, arg, errno);
	if (config_file.fail())
		throw configException("Error details: ", 0, strerror(errno), errno);
	this->_filepath = arg;
	_readFile(config_file);
}

void configParser::_readFile(std::ifstream &file_conf) {
	std::string line;


	getline(file_conf, line);
	while (getline(file_conf, line)) {
		
	}
}

void configParser::_tokenize() {

}

void configParser::_parseServer() {

}

void configParser::_parseLocation() {

}

void configParser::_parseDirective() {

}

void configParser::_expect() {

}

void configParser::_peek() {

}

void configParser::_consume() {

}

void configParser::_validateAll() {

}
