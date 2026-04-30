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
};