#include <configParser.hpp>
#include <configException.hpp>

#include <cstring>
#include <fstream>
#include <iostream>

// namespace { void trim(std::string &str)
// {
// 	size_t start = str.find_first_not_of(" \t\r\n");
// 	size_t end = str.find_last_not_of(" \t\r\n");

// 	if (start == std::string::npos || end == std::string::npos)
// 		str.clear();
// 	else
// 		str = str.substr(start, end - start + 1);
// } }


configParser::configParser() : _filepath(), _content(), _tokens(), _pos(0) /*, _servers()*/ {}

configParser::configParser(const configParser &rhs) : _filepath(rhs._filepath), _content(rhs._content), _tokens(rhs._tokens), _pos(rhs._pos) /*, _servers(rhs._servers)*/ {}

configParser::~configParser() {}

configParser &configParser::operator=(const configParser &rhs) {
	if (this != &rhs) {
		this->_filepath = rhs._filepath;
		this->_content = rhs._content;
		this->_tokens = rhs._tokens;
		this->_pos = rhs._pos;
		// this->_servers = rhs._servers;
	}
	return (*this);
}

// const std::vector<serverConfig> &configParser::getServers(void) {
// 	return (this->_servers);
// }

void configParser::parse(const std::string &arg) {
	if (arg.empty())
		throw configException("Error: empty arg ", 0, arg, errno);
	std::ofstream config_file(arg.c_str());
	if (!config_file.is_open())
		throw configException("Error: cannot open config file", 0, arg, errno);
	if (config_file.fail())
		throw configException("Error details: ", 0, strerror(errno), errno);
	this->_filepath = arg;
	_readFile(config_file);
}

void configParser::_readFile(std::ofstream &file_conf) {
	std::string	to_tokenize;

	std::streambuf *line = file_conf.rdbuf();
	_tokenize(to_tokenize);
}

void configParser::_tokenize(std::streambuf &str_to_token) {
	std::cout << str_to_token << std::endl;
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
