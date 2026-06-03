#pragma once

#include <string>
#include <sstream>

enum MIME { JPG, PNG, HTML, TXT, APP};

template <typename T> 
std::string toStr(T nbr) {
	std::ostringstream os;
	os << nbr;
	std::string ret = os.str();
	return (ret);
}

std::string findExt(const std::string &path);
MIME find_type(const std::string &filePath);