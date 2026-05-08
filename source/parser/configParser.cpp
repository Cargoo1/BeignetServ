#include <configParser.hpp>
#include <configException.hpp>
#include <parserUtils.hpp>

#include <algorithm>
#include <cstring>
#include <fstream>
#include <sstream>
#include <iostream>

#define PORT_MIN 1
#define PORT_MAX 65535
#define ERRO_MIN 400
#define ERRO_MAX 599

/*============= UTILS SERVER =============*/

enum DIR { LISTEN, SERVER_NAME, ERROR_PAGE, INDEX, CLIENT_MAX_BODY, NONE};

namespace { DIR findDir(const std::string &listenPort) {
	if (listenPort == "listen") return (LISTEN);
	if (listenPort == "server_name") return (SERVER_NAME);
	if (listenPort == "error_page") return (ERROR_PAGE);
	if (listenPort == "index") return (INDEX);
	if (listenPort == "client_max_body_size") return (CLIENT_MAX_BODY);
	else return (NONE);
} } 

namespace { bool parseDigit_code(const std::string &listenPort, int min, int max) {
	if (!string_verifFunc(listenPort, isdigit))
		return (false);
	int verif;
	std::stringstream ss(listenPort);
	ss >> verif;
	if (verif < min || verif > max)
		return (false);
	return (true);
} }

namespace { bool isValidPath(const std::string &token, bool absolute) {
	if (absolute ? (token.at(0) != '/') : (token.at(0) == '/'))
		return (false);
	if (!string_verifFunc(token, isspecial)) return (false);
	if (token.at(token.size()-1) == '/') return (false);
	return (true);
} }

namespace { bool check_double(const std::vector<unsigned int> &vect) {
	std::vector<unsigned int> tmp(vect);
	std::vector<unsigned int>::iterator it = tmp.begin();
	std::vector<unsigned int>::iterator ite = tmp.end();
	std::sort(it, ite);
	std::vector<unsigned int>::iterator comp = std::adjacent_find(it, ite);
	if (comp != ite) {
		std::cerr << "Error: duplicate entry: " << *comp << std::endl;
		return (false);
	}
	return (true);
} }

namespace { bool isValid_clientBodySisze(const std::string &token) {
	std::size_t i = 0;
	while (isdigit(token.at(i)))
		i++;
	char unite;
	unite = tolower(token.at(i));
	if (unite == 'k' || unite == 'm' || unite == 'g')
		i++;
	return (i == token.length());
} }

/*============= METHODE SERVER =============*/

configParser::configParser() : _content(), _tokens(), _pos(0), _servers() {}

configParser::configParser(const configParser &rhs) : _content(rhs._content), _tokens(rhs._tokens), _pos(rhs._pos), _servers(rhs._servers) {}

configParser::~configParser() {}

configParser &configParser::operator=(const configParser &rhs) {
	if (this != &rhs) {
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
	_readFile(arg);
	_tokenize();
	while (_pos < _tokens.size()) {
		_expect("server");
		serverConfig servTo_add;
		_parseServer(servTo_add);
		this->_servers.push_back(servTo_add);
	}
	_validateAll();
}

void configParser::_readFile(const std::string &to_token) {
	if (to_token.empty())
		throw configException("Error: empty to_token ", 0, to_token, errno);
	std::ifstream config_file(to_token.c_str());
	std::stringstream buffer;
	if (!config_file.is_open())
		throw configException("Error: cannot open config file", 0, to_token, errno);
	if (config_file.fail())
		throw configException("Error details: ", 0, strerror(errno), errno);
	buffer << config_file.rdbuf();
	this->_content = buffer.str();
}

void configParser::_tokenize() {
	std::size_t	ft_index = 0, nl = 1;
	std::string	current_word;
	Token		to_add;

	while (ft_index < this->_content.size()) {
		if (isblank(this->_content.at(ft_index)))
			ft_index++;
		else if (_content.at(ft_index) == '\n')
			ft_index++, nl++;
		else if (this->_content.at(ft_index) == '#') {
			while (ft_index < this->_content.size() && _content.at(ft_index) != '\n')
				ft_index++;
		}
		else if (isgraph(_content.at(ft_index))) {
			while (ft_index < this->_content.size() && isgraph(_content.at(ft_index))) {
				if (_content.at(ft_index) == ';' || _content.at(ft_index) == '{' || _content.at(ft_index) == '}') {
					if (!current_word.empty()) {
						to_add.update(current_word, nl);
						_tokens.push_back(to_add);
						current_word.clear();
					}
					current_word = _content.at(ft_index);
					to_add.update(current_word, nl);
					_tokens.push_back(to_add);
					current_word.clear();
					ft_index++;
				}
				else {
					current_word += _content.at(ft_index);
					ft_index++;
				}
			}
			if (!current_word.empty()) {
				to_add.update(current_word, nl);
				_tokens.push_back(to_add);
				current_word.clear();
			}
		}
	}
}

void configParser::_parseServer(serverConfig &servTo_pars) {
	_expect("{");
	while (_peek().getValue() != "}") {
		if (_peek().getValue() == "location") {
			_pos++;
			locationConfig locTo_add;
			_parseLocation(locTo_add);
			servTo_pars._locations.push_back(locTo_add);
		}
		else
			_parseDirective(servTo_pars);
	}
	_expect("}");
}

void configParser::_parseLocation(locationConfig &locTo_add) {
	if (_peek().getValue() != "/" || !isValidPath(_peek().getValue(), true))
		throw configException("Error: location path syntax:", _peek().getLine(), _peek().getValue());
	locTo_add._path = _consume().getValue();
}


void configParser::_parseDirective(serverConfig &toParse) {
	Token tmp = _peek();
	switch (findDir(_consume().getValue()))
	{
		case LISTEN: {
			if (!parseDigit_code(_peek().getValue(), PORT_MIN, PORT_MAX))
				throw configException("Error: listen port syntax error:", _peek().getLine(), _peek().getValue());
			toParse._port = _consume().getValue();
			_expect(";");
			break;
		}
		case SERVER_NAME: {
				if (!string_verifFunc(_peek().getValue(), isspecial))
					throw configException("Error: server name syntax error:", _peek().getLine(), _peek().getValue());
			toParse._host = _consume().getValue();
			_expect(";");
			break;
		}
		case ERROR_PAGE: {
			std::vector<unsigned int> codes;
			while (!isValidPath(_peek().getValue(), true)) {
				if (parseDigit_code(_peek().getValue(), ERRO_MIN, ERRO_MAX)) {
					std::stringstream ss(_consume().getValue());
					unsigned int toPush;
					ss >> toPush;
					codes.push_back(toPush);
				}
				else
					throw configException("Error: error_codes syntax:", _peek().getLine(), _peek().getValue());
			}
			if (!check_double(codes))
				throw configException("Error: duplicate error_codes", _peek().getLine());
			std::string url = _consume().getValue();
			for (std::size_t code = 0; code < codes.size(); code++)
				toParse._errorPages[code] = url;
			_expect(";");
			break;
		}
		case INDEX: {
			if (!isValidPath(_peek().getValue(), false))
				throw configException("Error: index syntax:", _peek().getLine(), _peek().getValue());
			toParse._index = _consume().getValue();
			_expect(";");
			break;
		}
		case CLIENT_MAX_BODY: {
			if(!isValid_clientBodySisze(_peek().getValue()))
				throw configException("Error: client_max_body_size syntax:", _peek().getLine(), _peek().getValue());
			std::stringstream ss(_consume().getValue());
				std::size_t clientBodySize;
				ss >> clientBodySize;
			toParse._clientMaxBodySize = clientBodySize;
			_expect(";");
			break;
		}
		default:
			throw configException("Error: server directive unknown: ", tmp.getLine(), tmp.getValue());
	}
}

void configParser::_expect(const std::string &to_compare) {
	if (_peek().getValue() != to_compare)
		throw configException("Error: syntax", _peek().getLine(), _peek().getValue());
	else
		_pos++;
}

Token configParser::_peek() {
	if (this->_pos >= _tokens.size())
		throw configException("Error: Unexpected end of file");
	return (this->_tokens[_pos]);
}

Token configParser::_consume() {
	Token tmp(this->_tokens[_pos]);
	_pos++;
	return (tmp);
}

void configParser::_validateAll() {
	if (_pos == _tokens.size()) {

	}

}
