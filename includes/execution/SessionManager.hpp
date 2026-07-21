#pragma once

#include <map>
#include <string>

class SessionManager {
	public:
		SessionManager(void) {};
		~SessionManager(void) {};

		void	setUserCounter(const std::string &idUser, unsigned int counter);

		unsigned int	getUserCounter(const std::string &idUser);

		void	restCounter(const std::string &idUser);
		void	incrCounter(const std::string &idUser);

	private:
		std::map <const std::string, unsigned int>	_usersData;
};