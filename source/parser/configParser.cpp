#include <configParser.hpp>
#include <configException.hpp>
#include <parserUtils.hpp>

#include <algorithm>
#include <cstring>
#include <fstream>
#include <climits>
#include <iostream>



#define PORT_MIN 1
#define PORT_MAX 65535
#define ERRO_MIN 400
#define ERRO_MAX 599
#define RDIR_MIN 300
#define RDIR_MAX 399

/*============= UTILS SERVER =============*/

namespace { DIR			findDir(const std::string &directive) {
	if (directive == "listen") return (LISTEN);
	if (directive == "server_name") return (SERVER_NAME);
	if (directive == "error_page") return (ERROR_PAGE);
	if (directive == "index") return (INDEX);
	if (directive == "client_max_body_size") return (CLIENT_MAX_BODY);
	else return (NONE);
} } 

namespace { L_CONF		find_LocDir(const std::string &local_confDir) {
	if (local_confDir == "methods") return (METHOD);
	if (local_confDir == "root") return (ROOT);
	if (local_confDir == "index") return (L_INDEX);
	if (local_confDir == "autoindex") return (AUTO_I);
	if (local_confDir == "upload_store") return (UPLD_S);
	if (local_confDir == "cgi") return (CGI);
	if (local_confDir == "return") return (REDIR);
	if (local_confDir == "client_max_body_size") return (L_CLIENT_MAX_BODY);
	else return (L_NONE);
} } 

namespace { bool		parseDigit_code(const std::string &listenPort, int min, int max) {
	if (!string_verifFunc(listenPort, isdigit))
		return (false);
	int verif;
	std::stringstream ss(listenPort);
	ss >> verif;
	if (verif < min || verif > max)
		return (false);
	return (true);
} }

namespace { bool		isValidPath(const std::string &token, bool absolute) {
	if (absolute ? (token.at(0) != '/') : (token.at(0) == '/'))
		return (false);
	if (!string_verifFunc(token, isspecial)) return (false);
	if (token.at(token.size()-1) == '/') return (false);
	return (true);
} }

namespace { bool		isValidUrl(const std::string &token) {
	if (token.at(0) == '/')
		return (isValidPath(token, true));
	return(isValidPath(token, false));
} }

namespace { bool		isValid_clientBodySisze(const std::string &token) {
	std::size_t i = 0;
	while (isdigit(token.at(i)))
		i++;
	char unite = tolower(token.at(i));
	if (unite == 'k' || unite == 'm' || unite == 'g')
		i++;
	return (i == token.length());
} }

namespace { bool		isValid_extCgi(const std::string &token) {
	std::size_t i = 1;
	if (token.at(0) != '.')
		return (false);
	while (i < token.length() && islower(token.at(i)))
		i++;
	return (i == token.length());
} }

namespace { std::size_t	convert_clientBodyS(const std::string &conv) {
	std::size_t ret = toInt(conv);
	std::size_t max_size = (size_t)-1;

	switch (conv.at(conv.size()-1))
	{
		case 'k':
			if (ret * 1024 > max_size)
				throw std::out_of_range("Error: risk overflow client_max_body_size");
			ret = ret * 1024;
			break;
		case 'm':
			if (ret * 1024 * 1024 > max_size)
				throw std::out_of_range("Error: risk overflow client_max_body_size");
			ret = ret * 1024 * 1024;
			break;
		case 'g':
			if (ret * 1024 * 1024 * 1024 > max_size)
				throw std::out_of_range("Error: risk overflow client_max_body_size");
			ret = ret * 1024 * 1024 * 1024;;
			break;
		default:
			break;
	}
	return (ret);
} }

namespace { bool		isValid_method(const std::vector<std::string> &methods) {
	std::vector<std::string>::const_iterator it = methods.begin();
	while (it != methods.end()) {
		if (*it == "GET") it++;
		else if (*it == "POST") it++;
		else if (*it == "DELETE") it++;
		else if (*it == "PUT") it++;
		else if (*it == "HEAD") it++;
		else if (*it == "OPIONS") it++;
		else if (*it == "PATCH") it++;
		else
			return false;
	}
	return (true);
} }

/*============= METHODE SERVER =============*/
/*============= cnstr/dstr =============*/

configParser::configParser() : _content(), _tokens(), _pos(0), _servers() {}

configParser::configParser(const configParser &rhs) : _content(rhs._content), _tokens(rhs._tokens), _pos(rhs._pos), _servers(rhs._servers) {}

configParser::~configParser() {}

/*============= oprtr/getr =============*/

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

/*============= parsr =============*/

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
	_pos++;
}

void configParser::_parseLocation(locationConfig &locTo_add) {
	if (_peek().getValue() != "/" && !isValidPath(_peek().getValue(), true))
		throw configException("Error: location path syntax:", _peek().getLine(), _peek().getValue());
	locTo_add._path = _consume().getValue();
	_expect("{");
	while (_peek().getValue() != "}") {
		_parseLocationDir(locTo_add);
	}
	_pos++;
}

void configParser::_parseLocationDir(locationConfig &locTo_pars) {
	Token tmp = _peek();
	switch (find_LocDir(_consume().getValue()))
	{
		case METHOD: {
			std::vector<std::string> methds;
			while (_peek().getValue() != ";") {
				if (string_verifFunc(_peek().getValue(), isupper))
					methds.push_back(_consume().getValue());
				else
					throw configException("Error: methods syntax:", _peek().getLine(), _peek().getValue());
			}
			locTo_pars._methods = methds;
			_expect(";");
			break;
		}
		case ROOT: {
			if (!isValidPath(_peek().getValue(), true))
				throw configException("Error: root path syntax:", _peek().getLine(), _peek().getValue());
			locTo_pars._root = _consume().getValue();
			_expect(";");
			break;
		}
		case L_INDEX: {
			if (!isValidPath(_peek().getValue(), false))
				throw configException("Error: index syntax:", _peek().getLine(), _peek().getValue());
			locTo_pars._index = _consume().getValue();
			_expect(";");
			break;
		}
		case AUTO_I: {
			if (_peek().getValue() != "on" && _peek().getValue() != "off")
				throw configException("Error: autoindex syntax:", _peek().getLine(), _peek().getValue());
			locTo_pars._autoindex = (_consume().getValue() == "on");
			_expect(";");
			break;
		}
		case UPLD_S: {
			if (!isValidPath(_peek().getValue(), true))
				throw configException("Error: upload_store syntax:", _peek().getLine(), _peek().getValue());
			locTo_pars._uploadStore = _consume().getValue();
			_expect(";");
			break;
		}
		case CGI: {
			if (!isValid_extCgi(_peek().getValue()))
				throw configException("Error: cgi extention syntax:", _peek().getLine(), _peek().getValue());
			std::string ext = _consume().getValue();
			if (!isValidPath(_peek().getValue(), true))
				throw configException("Error: cgi path syntax:", _peek().getLine(), _peek().getValue());
			locTo_pars._cgi[ext] = _consume().getValue();
			_expect(";");
			break;
		}
		case REDIR: {
			if (!string_verifFunc(_peek().getValue(), isdigit))
				throw configException("Error: redir code must be numerics characters:", _peek().getLine(), _peek().getValue());
			int code = toInt(_consume().getValue());
			if (_peek().getValue() != ";") {
				if (!isValidUrl(_peek().getValue()))
					throw configException("Error: redir url syntax:", _peek().getLine(), _peek().getValue());
				locTo_pars._redirectUrl = _consume().getValue();
				if (code < RDIR_MIN || code > RDIR_MAX)
					throw configException("Error: redir code to url must be 3xx:", _peek().getLine(), _peek().getValue());
			}
			locTo_pars._redirectCode = code;
			locTo_pars._hasRedirect = true;
			_expect(";");
			break;
		}
		case L_CLIENT_MAX_BODY: {
			if(!isValid_clientBodySisze(_peek().getValue()))
				throw configException("Error: client_max_body_size syntax:", _peek().getLine(), _peek().getValue());
			locTo_pars._clientMaxBodySize = convert_clientBodyS(_consume().getValue());
			_expect(";");
			break;
		}
		default:
			throw configException("Error: location directive unknown: ", tmp.getLine(), tmp.getValue());
	}
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
			toParse._serverName = _consume().getValue();
			_expect(";");
			break;
		}
		case ERROR_PAGE: {
			std::vector<unsigned int> codes;
			while (!isValidPath(_peek().getValue(), true)) {
				if (parseDigit_code(_peek().getValue(), ERRO_MIN, ERRO_MAX)) {
					codes.push_back(toInt(_consume().getValue()));
				}
				else
					throw configException("Error: error_codes syntax:", _peek().getLine(), _peek().getValue());
			}
			if (!check_double(codes))
				throw configException("Error: duplicate error_codes", _peek().getLine());
			std::string url = _consume().getValue();
			for (std::size_t code = 0; code < codes.size(); code++)
				toParse._errorPages[codes[code]] = url;
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
			toParse._clientMaxBodySize = convert_clientBodyS(_consume().getValue());
			_expect(";");
			break;
		}
		default:
			throw configException("Error: server directive unknown: ", tmp.getLine(), tmp.getValue());
	}
}

void configParser::_validateAll() {
	if (_servers.size() <= 0) {
		std::cerr << "Error: config must have at least one server conf" << std::endl;
		throw std::runtime_error("Aborting");
	}
	std::vector<std::string> port_dup;
	std::vector<std::string> path_dup;
	for (std::size_t i = 0; i < this->_servers.size(); i++) {
		const serverConfig &server = this->_servers[i];
		if (server._port.empty()) {
			std::cerr << "Error: server " << server._serverName << " has no port define" << std::endl;
			throw std::runtime_error("Aborting");
		}
		port_dup.push_back(server._port);
		if (!check_double(port_dup)) {
			std::cerr << "Error: server " << server._serverName << " is listening an already assigned port" << std::endl;
			throw std::runtime_error("Aborting");
		}
		for (std::size_t j = 0; j < server._locations.size(); j++) {
			const locationConfig &location = server._locations[j];
			path_dup.push_back(location._path);
			if (!check_double(path_dup)) {
				std::cerr << "Error: server " << server._serverName << " have locations with duplicate path:" << location._path << std::endl;
				throw std::runtime_error("Aborting");
			}
			if (!isValid_method(location._methods)) {
				std::cerr << "Error: server " << server._serverName << ": " << location._path << ": have unknown methodes" << std::endl;
				throw std::runtime_error("Aborting");
			}
		}
		path_dup.clear();
	}
}

/*============= tokens =============*/

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


void	configParser::DEBUG_printConf(void) {
	std::cout << "\n=============== PARSED CONFIGURATION ===============\n" << std::endl;
	
	for (std::size_t i = 0; i < this->_servers.size(); i++) {
		const serverConfig &server = this->_servers[i];
		
		std::cout << "--- SERVER " << i + 1 << " ---" << std::endl;

		std::cout << "  Port: " << server._port << std::endl;
		std::cout << "  Server Name: " << server._serverName << std::endl;
		std::cout << "  Index: " << server._index << std::endl;
		std::cout << "  Client Max Body Size: " << server._clientMaxBodySize << std::endl;
		
		if (!server._errorPages.empty()) {
			std::cout << "  Error Pages:" << std::endl;
			for (std::map<int, std::string>::const_iterator it = server._errorPages.begin(); 
				 it != server._errorPages.end(); ++it) {
				std::cout << "    " << it->first << " -> " << it->second << std::endl;
			}
		}
		
		if (!server._locations.empty()) {
			std::cout << "  Locations:" << std::endl;
			for (std::size_t j = 0; j < server._locations.size(); j++) {
				const locationConfig &loc = server._locations[j];
				std::cout << "    [" << j + 1 << "] Path: " << loc._path << std::endl;
				
				if (!loc._methods.empty()) {
					std::cout << "        Methods: ";
					for (std::size_t k = 0; k < loc._methods.size(); k++) {
						std::cout << loc._methods[k];
						if (k < loc._methods.size() - 1) std::cout << ", ";
					}
					std::cout << std::endl;
				}
				
				if (!loc._root.empty())
					std::cout << "        Root: " << loc._root << std::endl;
				if (!loc._index.empty())
					std::cout << "        Index: " << loc._index << std::endl;
				std::cout << "        Autoindex: " << (loc._autoindex ? "on" : "off") << std::endl;
				if (!loc._uploadStore.empty())
					std::cout << "        Upload Store: " << loc._uploadStore << std::endl;
				
				if (!loc._cgi.empty()) {
					std::cout << "        CGI:" << std::endl;
					for (std::map<std::string, std::string>::const_iterator it = loc._cgi.begin();
						 it != loc._cgi.end(); ++it) {
						std::cout << "          " << it->first << " -> " << it->second << std::endl;
					}
				}
				
				if (loc._hasRedirect) {
					std::cout << "        Redirect: " << loc._redirectCode;
					if (!loc._redirectUrl.empty())
						std::cout << " -> " << loc._redirectUrl;
					std::cout << std::endl;
				}
				
				if (loc._hasClientMaxBodySize)
					std::cout << "        Client Max Body Size: " << loc._clientMaxBodySize << std::endl;
			}
		}
		std::cout << std::endl;
	}
	std::cout << "==================================================\n" << std::endl;
}