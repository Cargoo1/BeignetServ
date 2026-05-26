#pragma once

#include <sstream>
#include <string>
#include <map>

enum MIME { JPG, PNG, HTML, TXT, APP};

enum	client_error
{
	bad_request = 400,
	forbiden = 403,
	not_found,
	method_not_allowed,
	payload_too_large = 415,
	internal_server_error = 500
};

class HttpResponse {
	public:
		HttpResponse();
		HttpResponse(int code);
		HttpResponse(const HttpResponse &rhs);
		~HttpResponse();
		HttpResponse &operator=(const HttpResponse &rhs);

		void	setStatusCode(int code);
		void	addField(const std::string &key, const std::string &value);
		void	setBody(const std::string &body);

		void	addContentLength();
		void	setContentType(std::string type);
		bool	setBodyFromFile(const std::string &filepath);

		int											getStatusCode();
		const std::map<std::string, std::string>	&getHeader();
		const std::string							&getBody();
		bool										getField(const std::string &key, std::string& value);

		std::string	toHttpString();

	private:
		int										_statusCode;
		std::map<std:: string, std:: string>	_header;
		std::string								_body;
};

/* ==== utils ==== */
template <typename T>
std::string toStr(const T &num) {
	std::stringstream	oss;
	oss << num;
	return (oss.str());
}
