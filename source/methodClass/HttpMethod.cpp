#include <HttpMethod.hpp>

#include <bits/stdc++.h>
#include <vector>
#include <sys/stat.h>


// namespace { std::string findRootDir(std::string path) {
// 	std::string del = "/";
// 	std::string::size_type pos = path.find(del);
// 	if (path.substr(0, pos) == "") {
// 		path.erase(0, pos + del.length());
// 		pos = path.find(del);
// 	}
// 	return (path.substr(0, pos));
// } }

HttpMethod::HttpMethod(const ExecutionContext &context) : _context(context) {}

std::string HttpMethod::NormalizePath(std::string toNomalize) {
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

struct stat HttpMethod::getFileData(const std::string &filePath) {
	struct stat buffer;
	stat(filePath.c_str(), &buffer);
	return (buffer); 
}

std::string HttpMethod::getContentType(const std::string &filePath) {
	MIME ext = find_type(filePath);
	switch (ext)
	{
		case JPG:
			return ("image/jpeg");
		case PNG:
			return ("image/png");
		case HTML:
			return ("text/html");
		case TXT:
			return ("text/plain");
		default:
			return ("application/octet-stream");
	}
}

HttpResponse HttpMethod::buildErrorResponse(HttpResponse &res, int code, std::string msg) {
	HttpResponse ret(code);
	
}
