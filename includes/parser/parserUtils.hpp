#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iostream>

bool string_verifFunc(const std::string &str, int f(int));
int isspecial(int c);

unsigned int toInt(const std::string &str);

template <typename T>
bool check_double(const std::vector<T> &vect) {
	std::vector<T> tmp(vect);
	typename std::vector<T>::iterator it = tmp.begin();
	typename std::vector<T>::iterator ite = tmp.end();
	std::sort(it, ite);
	typename std::vector<T>::iterator comp = std::adjacent_find(it, ite);
	if (comp != ite) {
		return (false);
	}
	return (true);
}