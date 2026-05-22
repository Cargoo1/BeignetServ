#include <locationConfig.hpp>

locationConfig::locationConfig() : _autoindex(false), _hasRedirect(false), _hasClientMaxBodySize(false) {}
locationConfig::locationConfig(const locationConfig &rhs) : _path(rhs._path), _methods(rhs._methods), _root(rhs._root), _index(rhs._index), _autoindex(rhs._autoindex), _uploadStore(rhs._uploadStore), _cgi(rhs._cgi), _hasRedirect(rhs._hasRedirect), _redirectCode(rhs._redirectCode), _redirectUrl(rhs._redirectUrl), _clientMaxBodySize(rhs._clientMaxBodySize), _hasClientMaxBodySize(rhs._hasClientMaxBodySize) {}
locationConfig::~locationConfig() {}

locationConfig &locationConfig::operator=(const locationConfig &rhs) {
		if (this != &rhs) {
		this->_path = rhs._path;
		this->_methods = rhs._methods;
		this->_root = rhs._root;
		this->_index = rhs._index;
		this->_autoindex = rhs._autoindex;
		this->_uploadStore = rhs._uploadStore;
		this->_cgi = rhs._cgi; 
		this->_hasRedirect = rhs._hasRedirect; 
		this->_redirectCode = rhs._redirectCode; 
		this->_redirectUrl = rhs._redirectUrl; 
		this->_clientMaxBodySize = rhs._clientMaxBodySize; 
		this->_hasClientMaxBodySize = rhs._hasClientMaxBodySize;
	}
	return (*this);
}

std::string &locationConfig::getPath() {
	return (this->_path);
}

const std::string &locationConfig::getPath() const {
	return (this->_path);
}

std::string &locationConfig::getRoot() {
	return (this->_root);
}

const std::string &locationConfig::getRoot() const {
	return (this->_root);
}