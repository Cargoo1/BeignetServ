#include "../includes/Config.hpp"
#include <map>
#include <string>

Config::Config() {

}

Config::Config(const Config &rhs) {
	(void)rhs;
}

Config::~Config() {

}

Config &Config::operator=(const Config &rhs) {
	(void)rhs;
	return (*this);
}

const servers_list	&Config::getServers(void)
{
	return (this->_servers);
}
