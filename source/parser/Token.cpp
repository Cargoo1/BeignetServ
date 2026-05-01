#include <Token.hpp>

Token::Token() {}

Token::Token(const std::string &value, int line) : _value(value), _line(line) {}

Token::Token(const Token &rhs) : _value(rhs._value), _line(rhs._line) {}

Token &Token::operator=(const Token &rhs) {
	if (this != &rhs) {
		this->_value = rhs._value;
		this->_line = rhs._line;
	}
	return *this;
}

Token::~Token() {}


const std::string &Token::getValue() const {
	return (this->_value);
}

int Token::getLine() const {
	return (this->_line);
}
