#include <configException.hpp>
#include <sstream>

configException::configException(const std::string &message, int line, const std::string &file, int code)
	: _message(message), _file(file), _line(line), _code(code), _formatted()
{
	std::ostringstream out;
	out << _message;
	if (!_file.empty()) {
		out << " (" << _file;
		if (_line >= 0)
			out << ":" << _line;
		out << ")";
	} else if (_line >= 0) {
		out << " at line " << _line;
	}
	if (_code != 0) {
		out << " [code " << _code << "]";
	}
	_formatted = out.str();
}

configException::configException(const configException &other)
	: _message(other._message), _file(other._file), _line(other._line), _code(other._code), _formatted(other._formatted)
{
}

configException &configException::operator=(const configException &other) {
	if (this != &other) {
		_message = other._message;
		_file = other._file;
		_line = other._line;
		_code = other._code;
		_formatted = other._formatted;
	}
	return *this;
}

configException::~configException() throw() {}

const char* configException::what() const throw() {
	return _formatted.c_str();
}

const std::string &configException::message() const { return _message; }
const std::string &configException::file() const { return _file; }
int configException::line() const { return _line; }
int configException::code() const { return _code; }