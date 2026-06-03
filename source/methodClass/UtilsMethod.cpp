#include <UtilsMethod.hpp>

std::string findExt(const std::string &path) {
	std::string ret = path;
	std::string del = ".";
	std::string::size_type pos = ret.find(del);
	while (pos != std::string::npos) {
		ret.erase(0, pos + del.length());
		pos = ret.find(del);
	}
	return (ret);
}

MIME find_type(const std::string &filePath)
{
	std::string ext = findExt(filePath);
	if (ext == "jpeg")
		return (JPG);
	else if (ext == "png")
		return (PNG);
	else if (ext == "html")
		return (HTML);
	else if (ext == "txt")
		return (TXT);
	else
		return (APP);
}