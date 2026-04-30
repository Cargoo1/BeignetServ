#pragma once

#include <string> 

class Token {
	public:
		Token();
		Token(const std::string &value, int line);
		Token(const Token &other);
		Token &operator=(const Token &other);
		~Token();

		const std::string	&getValue() const;
		int					getLine()  const;

	private:
		std::string	_value;
		int			_line;
};