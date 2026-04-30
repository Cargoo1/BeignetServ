#pragma once

#include <exception>
#include <string>

class configException : public std::exception {
	private:
		std::string	_message;
	public:
		configException(const std::string &message, int line);
		const char* what() const throw();
};
