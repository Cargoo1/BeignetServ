#pragma once

#include <map>
#include <vector>
#include <string>

enum L_CONF { METHOD, ROOT, L_INDEX, AUTO_I, UPLD_S, CGI, REDIR, L_CLIENT_MAX_BODY, L_NONE};
enum MTHD { GET, POST, DELETE, PUT, HEAD, OPTIONS, PATCH};

class locationConfig {
	public:
		friend class configParser;
		locationConfig();
		locationConfig(const locationConfig &rhs);
		~locationConfig();

		locationConfig &operator=(const locationConfig &rhs);

		std::string &getPath();
		const std::string &getPath() const;

		std::string &getRoot();
		const std::string &getRoot() const;

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
		std::size_t							_clientMaxBodySize;
		bool								_hasClientMaxBodySize;
};