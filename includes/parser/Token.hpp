#pragma once

#include <string>
#include <typeinfo>

class Token {
	public:
		Token();
		Token(const std::string &value, int line);
		Token(const Token &other);
		Token &operator=(const Token &other);
		~Token();

		const std::string	&getValue() const;
		int					getLine()  const;

		void				update(const std::string &value, int line);

	private:
		std::string	_value;
		int			_line;
};