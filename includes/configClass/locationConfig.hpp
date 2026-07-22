#pragma once

#include <map>
#include <vector>
#include <string>

enum L_CONF { METHOD, L_ROOT, L_INDEX, INDEX_CGI, AUTO_I, UPLD_S, CGI, REDIR, L_CLIENT_MAX_BODY, L_NONE};
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

		bool const& hasIndex() const;
	
		std::string &getIndex();
		const std::string &getIndex() const;


		std::string &getRoot();
		const std::string &getRoot() const;

		std::string &getUploadStore();
		const std::string &getUploadStore() const;

		bool const& hasCMBS() const;

		bool const& hasRedirect() const;

		bool const& getAutoindex() const;

		std::size_t &getCMBS();
		const std::size_t &getCMBS() const;

		int &getRedirectCode();
		const int &getRedirectCode() const;

		std::string &getRedirectUrl();
		const std::string &getRedirectUrl() const;

		std::vector<std::string> &getMethod();
		const std::vector<std::string> &getMethod() const;

		std::map<std::string, std::string> &getCGI();
		const std::map<std::string, std::string> &getCGI() const;

		bool const& hasIndexCgi() const;

		std::string &getIndexCgi();
		const std::string &getIndexCgi() const;

	private:
		std::string							_path;
		std::vector<std::string>			_methods;
		std::string							_root;
		std::string							_index;
		bool								_hasIndex;
		bool								_autoindex;
		std::string							_uploadStore;
		std::map<std::string, std::string>	_cgi;
		std::string							_indexCgi;
		bool								_hasIndexCgi;
		bool								_hasRedirect;
		int									_redirectCode;
		std::string							_redirectUrl;
		std::size_t							_clientMaxBodySize;
		bool								_hasClientMaxBodySize;
};
