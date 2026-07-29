#include <locationConfig.hpp>

locationConfig::locationConfig() : _hasIndex(false), _autoindex(false), _isCgiBin(false), _hasIndexCgi(false), _hasRedirect(false), _hasClientMaxBodySize(false) {}
locationConfig::locationConfig(const locationConfig &rhs) : _path(rhs._path), _methods(rhs._methods), _root(rhs._root), _index(rhs._index), _hasIndex(rhs._hasIndex), _autoindex(rhs._autoindex), _isCgiBin(rhs._isCgiBin), _uploadStore(rhs._uploadStore), _cgi(rhs._cgi), _indexCgi(rhs._indexCgi), _hasIndexCgi(rhs._hasIndexCgi), _hasRedirect(rhs._hasRedirect), _redirectCode(rhs._redirectCode), _redirectUrl(rhs._redirectUrl), _clientMaxBodySize(rhs._clientMaxBodySize), _hasClientMaxBodySize(rhs._hasClientMaxBodySize) {}
locationConfig::~locationConfig() {}

locationConfig &locationConfig::operator=(const locationConfig &rhs) {
		if (this != &rhs) {
		this->_path = rhs._path;
		this->_methods = rhs._methods;
		this->_root = rhs._root;
		this->_index = rhs._index;
		this->_hasIndex = rhs._hasIndex;
		this->_autoindex = rhs._autoindex;
		this->_isCgiBin = rhs._isCgiBin;
		this->_uploadStore = rhs._uploadStore;
		this->_cgi = rhs._cgi;
		this->_indexCgi = rhs._indexCgi;
		this->_hasIndexCgi = rhs._hasIndexCgi;
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

bool const	&locationConfig::hasIndex() const {
	return (this->_hasIndex);
}

std::string &locationConfig::getIndex() {
	return (this->_index);
}

const std::string &locationConfig::getIndex() const {
	return (this->_index);
}

std::string &locationConfig::getRoot() {
	return (this->_root);
}

const std::string &locationConfig::getRoot() const {
	return (this->_root);
}

std::string &locationConfig::getUploadStore() {
	return (this->_uploadStore);
}

const std::string &locationConfig::getUploadStore() const {
	return (this->_uploadStore);
}

bool const& locationConfig::hasCMBS() const {
	return (this->_hasClientMaxBodySize);
}

bool const& locationConfig::isCgiBin() const {
	return (this->_isCgiBin);
}

bool const& locationConfig::hasRedirect() const {
	return (this->_hasRedirect);
}

bool const& locationConfig::getAutoindex() const {
	return (this->_autoindex);
}

unsigned long long &locationConfig::getCMBS() {
	return (this->_clientMaxBodySize);
}

const unsigned long long &locationConfig::getCMBS() const {
	return (this->_clientMaxBodySize);
}

int &locationConfig::getRedirectCode() {
	return (this->_redirectCode);
}

const int &locationConfig::getRedirectCode() const {
	return (this->_redirectCode);
}

std::string &locationConfig::getRedirectUrl() {
	return (this->_redirectUrl);
}

const std::string &locationConfig::getRedirectUrl() const {
	return (this->_redirectUrl);
}

std::vector<std::string> &locationConfig::getMethod() {
	return (this->_methods);
}

const std::vector<std::string> &locationConfig::getMethod() const {
	return (this->_methods);
}

std::map<std::string, std::string> &locationConfig::getCGI() {
	return (this->_cgi);
}

const std::map<std::string, std::string> &locationConfig::getCGI() const {
	return (this->_cgi);
}

bool const &locationConfig::hasIndexCgi() const {
	return (this->_hasIndexCgi);
}

std::string &locationConfig::getIndexCgi() {
	return (this->_indexCgi);
}

const std::string &locationConfig::getIndexCgi() const {
	return (this->_indexCgi);
}
