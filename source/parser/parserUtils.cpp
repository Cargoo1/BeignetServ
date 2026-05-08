#include <parserUtils.hpp>

bool string_verifFunc(const std::string &str, int f(int)) {
	for (std::size_t i = 0; i < str.size(); i++) {
		if(!f(str.at(i)))
			return (false);
	}
	return (true);
}

int isspecial(int c) {
	if (!isalnum(c) || c != '/' || c != '-' || c != '_' || c != '.')
		return (1);
	return (0);
}