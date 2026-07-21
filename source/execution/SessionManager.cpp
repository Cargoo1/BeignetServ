#include "SessionManager.hpp"
#include <limits>

void	SessionManager::setUserCounter(const std::string &idUser, unsigned int counter) {
	this->_usersData[idUser] = counter;
}

unsigned int	SessionManager::getUserCounter(const std::string &idUser) {
	return(this->_usersData[idUser]);
}

void	SessionManager::restCounter(const std::string &idUser) {
	this->_usersData[idUser] = 0;
}

void	SessionManager::incrCounter(const std::string &idUser) {
	if (this->_usersData[idUser] < std::numeric_limits<unsigned int>::max())
		this->_usersData[idUser] += 1;
	else
		this->_usersData[idUser] = 0;
}
