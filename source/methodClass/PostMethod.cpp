#include <PostMethod.hpp>
#include <UtilsMethod.hpp>

#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <ctime>
#include <sys/stat.h>





// template <typename T> std::string toStr(T nbr) {
// 	std::ostringstream os;
// 	os << nbr;
// 	std::string ret = os.str();
// 	return (ret);
// }

// namespace { std::string generateTimestampFilename(std::string filename){
// 	std::string::size_type pos = filename.find(".");
// 	std::cout << filename << std::endl;
// 	if (pos == std::string::npos) {
// 		std::cout << "Warning: not a file" << std::endl;
// 		return (filename);
// 	}
// 	std::string tmp = filename.substr(0, pos);
// 	std::time_t resultTime = std::time(0);
// 	std::string timestamp = toStr(resultTime);
// 	std::string newFileName = tmp + "-" + timestamp;
// 	tmp = filename.substr(pos);
// 	newFileName += tmp;
// 	return (newFileName);
// } }

// namespace { bool createFile(const std::string &fileName, const std::string &body) {
// 	std::ofstream ofs;
// 	ofs.open(fileName.c_str());
// 	if (!ofs.is_open()) {
// 		std::cerr << "creating " << fileName << " failed" << std::endl;
// 		return (false);
// 	}
// 	ofs.write(body.c_str(), body.size());
// 	ofs.close();
// 	return (true);
// } }

// namespace { bool postFile(std::string path, const std::string &body){
// 	std::size_t found = path.find_last_of("/\\");
// 	std::string newPath = path.substr(0, found);
// 	std::istringstream iss(newPath);
// 	std::string file = path.substr(found+1);
// 	std::string part, current_path = "";
// 	struct stat path_stat;


// 	if (found != std::string::npos) {
// 		while (std::getline(iss, part, '/')) {
// 			std::cout << part << std::endl;
// 			if (part.empty())
// 				continue;
// 			current_path += part;
// 			if (stat(current_path.c_str(), &path_stat) != 0) {
// 				if (mkdir(current_path.c_str(), S_IRWXU) != 0) {
// 					std::cerr << "Post method mkdir failed" << std::endl;
// 					return (false);
// 				}
// 			}
// 			current_path += "/";
// 		}
// 	}
// 	if (!file.empty()) {
// 		if (!current_path.empty())
// 			current_path += "/";
// 		current_path += file;
// 		if (stat(current_path.c_str(), &path_stat) != 0)
// 			file = generateTimestampFilename(file);
// 		if (!createFile(current_path, body)) {
// 			return (false);
// 		}
// 	}
// 	return (true);
// } }






namespace { std::size_t toSizeT(const std::string &str) {
	std::stringstream ss(str);
	std::size_t ret;
	ss >> ret;
	return (ret);
} }

namespace { std::string generateTimestampFilename(std::string filename){
	std::string::size_type pos = filename.find(".");
	if (pos == std::string::npos) {
		std::cout << "Warning: not a file" << std::endl;
		return (filename);
	}
	std::string tmp = filename.substr(0, pos);
	std::time_t resultTime = std::time(0);
	std::string timestamp = toStr(resultTime);
	std::string newFileName = tmp + "-" + timestamp;
	tmp = filename.substr(pos);
	newFileName += tmp;
	return (newFileName);
} }


namespace { bool isFile(const std::string &str) {
	std::string::size_type pos = str.find(".");
	if (pos == std::string::npos) {
		return (false);
	}
	std::string compare = str.substr(pos); 
	if (compare == ".txt" || compare == ".png" || compare == ".jpg" || compare == ".pdf")
		return (true);
	return (false);
} }

namespace { bool createFile(const std::string &fileName, const std::string &body) {
	std::ofstream ofs;
	struct stat path_stat;
	if (stat(fileName.c_str(), &path_stat) == 0) {
		std::cerr << fileName << " exist already" << std::endl;
		return (false);
	}
	ofs.open(fileName.c_str());
	if (!ofs.is_open()) {
		std::cerr << "creating " << fileName << " failed" << std::endl;
		return (false);
	}
	ofs.write(body.c_str(), body.size());
	ofs.close();
	return (true);
} }

namespace { bool postFile(std::string path, const std::string &body){
	std::istringstream iss(path);
	std::string part, current_path = "";
	struct stat path_stat;
	const std::size_t size = 1024; 

	while (std::getline(iss, part, '/')) {
		current_path += part;
		if (stat(current_path.c_str(), &path_stat) != 0) {
			if (mkdir(current_path.c_str(), S_IRWXU) != 0) {
				std::cerr << "Post method mkdir failed" << std::endl;
				return (false);
			}
		}
	}
	return (true);
} }

PostMethod::PostMethod(ExecutionContext &context) : HttpMethod(context) {};

PostMethod::~PostMethod() {};

void PostMethod::executeMethod(HttpResponse &rsp) {
	std::string path;
	if (this->_context.location.getUploadStore().empty())
		path = this->_context.location.getRoot() + this->_context.request.getHeader().getTargetResource();
	else
		path = this->_context.location.getUploadStore() + this->_context.request.getHeader().getTargetResource();
	std::size_t contentLenght = toSizeT(this->_context.request.getHeader().getContentLenght());
	if (this->_context.location.hasCMBS()) {
		if (contentLenght > this->_context.location.getCMBS())
			throw Request::ErrorRequest(content_too_large);
	}
	if (!postFile(path, this->_context.request.getBody()))
		throw Request::ErrorRequest(internal_server_error);
	
}