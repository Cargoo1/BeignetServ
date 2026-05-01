#pragma once

#include <exception>
#include <string>

class configException : public std::exception {
	private:
		std::string	_message;
		std::string	_file;
		int			_line;
		int			_code;
		std::string	_formatted;
	public:
		configException(const std::string &message, int line = -1, const std::string &file = "", int code = 0);
		configException(const configException &other);
		configException &operator=(const configException &other);
		virtual ~configException() throw();


		const char* what() const throw();

		const std::string &message() const;
		const std::string &file() const;
		int line() const;
		int code() const;
};
