#pragma once

#include <map>
#include <vector>
#include <string>

class locationConfig {
	public:
		locationConfig();
		locationConfig(const locationConfig &rhs);
		~locationConfig();

		locationConfig &operator=(const locationConfig &rhs);

	private:
		std::string							_path;
		std::vector<std::string>			_methods;
		std::string							_root;
		std::string							_index;
		bool								_autoindex;
		std::string							_uploadStore;
		std::map<std::string, std::string>	_cgi;
		bool								_hasRedirect;
		int									_redirectCode;
		std::string							_redirectUrl;
		size_t								_clientMaxBodySize;
		bool								_hasClientMaxBodySize;
};