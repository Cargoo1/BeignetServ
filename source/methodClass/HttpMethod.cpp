#include "Request.hpp"
#include <HttpMethod.hpp>

#include <bits/stdc++.h>
#include <vector>
#include <sys/stat.h>
#include <sys/wait.h>

HttpMethod::HttpMethod(Request const& r) : _request(r) {}

HttpMethod::~HttpMethod(){}

std::string HttpMethod::_NormalizePath(std::string toNomalize) {
	std::vector<std::string> vectStr;
	std::string tmp;
	for (std::size_t i = 0; i < toNomalize.size(); i++) {
		if (toNomalize.at(i) != '/')
			tmp += toNomalize.at(i);
		else {
			if (tmp == ".." && vectStr.size() > 0)
				vectStr.pop_back();
			else if (tmp != "")
				vectStr.push_back(tmp);
			tmp.clear();
		}
	}
	if (tmp == ".." && vectStr.size() > 0)
		vectStr.pop_back();
	else if (tmp != "")
		vectStr.push_back(tmp);
	tmp.clear();
	for (std::vector<std::string>::iterator it = vectStr.begin(); it != vectStr.end(); ++it) {
		tmp += "/" + *it;
	}
	return (tmp);
}

struct stat HttpMethod::_getFileData(const std::string &filePath) {
	struct stat buffer;
	stat(filePath.c_str(), &buffer);
	return (buffer); 
}

