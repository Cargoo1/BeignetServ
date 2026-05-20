#pragma once

#include <string>
#include <map>

class HttpResponse {
	public:
		HttpResponse();
		HttpResponse(int code);
		HttpResponse(const HttpResponse &rhs);
		~HttpResponse();
		HttpResponse &operator=(const HttpResponse &rhs);

		void	setStatusCode(int code);
		void	addHeaders(const std::string &key, const std::string &value);
		void	setBody(const std::string &body);

		void	addContentLength();
		void	setContentType(std::string type);
		bool	setBodyFromFile(const std::string &filepath);

		int											getStatusCode();
		const std::map<std::string, std::string>	&getHeaders();
		const std::string							&getBody();
		const std::string							*getHeader(const std::string &key);

		std::string	toHttpString();

	private:
		int										_statusCode;
		std::map<std:: string, std:: string>	_headers;
		std::string								_body;
};

/* ==== utils ==== */
template <typename T>
std::string toStr(const T &num) {
	std::ostringstream oss;
	oss << num;
	std::string	ret = oss.str();
	return (ret);
}