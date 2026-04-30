#include <configException.hpp>
#include <sstream> 

configException::configException(const std::string &message, int line) : _message (message) {
	std::ostringstream str;
	str << line;
	_message += " " + str.str();
}

const char* configException::what() const throw() {
	return _message.c_str();
}