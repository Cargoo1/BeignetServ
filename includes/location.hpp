#pragma once

#include <map>
#include <string>

class location {
	public:
		location();
		location(const location &rhs);
		~location();

		location &operator=(const location &rhs);

	private:
		std::map<unsigned int, std::string>	_locs;
};