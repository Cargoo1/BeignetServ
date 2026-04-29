#pragma once

#include <map>
#include <string>
#include <vector>

typedef std::vector<std::map<std::string, std::string>> servers_list;


class Config {
public:
	Config();
	Config(const Config &rhs);
	~Config();

	Config &operator=(const Config &rhs);
	const servers_list	&getServers(void);


private:
	servers_list	_servers;
};
