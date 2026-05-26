#include <HttpResponse.hpp>

#include <sstream>
#include <fstream>
#include <sys/stat.h>

/*============= UTILS SERVER =============*/

namespace { bool fileExist (const std::string& name) {
	struct stat buffer;
	return (stat (name.c_str(), &buffer) == 0); 
} }

namespace { const std::string getReasonPhrase(int code) {
	std::string	ret;
	switch (code)
	{
		
		case 201:
			ret = " Created\r\n";
			break;
		case 301: case 302:
			ret = " Redirect\r\n";
			break;
		case bad_request:
			ret = " Bad Request\r\n";
			break;
		case forbiden:
			ret = " Forbiden\r\n";
			break;
		case not_found:
			ret = " Not Found\r\n";
			break;
		case method_not_allowed:
			ret = " Method Not Allowed\r\n";
			break;
		case payload_too_large:
			ret = " Payload Too Large\r\n";
			break;
		case 500:
			ret = " Payload Too Large\r\n";
			break;
		default:
			ret = " OK\r\n";
			break;
	}
	return (ret);
} }

namespace { const std::string FieldsToResponse(const std::map<std::string, std::string> &map_to_str) {
	std::string ret;
	std::map<std::string, std::string>::const_iterator it = map_to_str.begin();
	for (;it != map_to_str.end(); ++it) {
		ret += it->first + ": " + it->second + "\r\n";
	}
	return (ret);
} }

/*============= METHODE HttpResponse =============*/
/*============= cnstr/dstr =============*/
HttpResponse::HttpResponse() : _statusCode(200) {}

HttpResponse::HttpResponse(int code) : _statusCode(code) {}

HttpResponse::HttpResponse(const HttpResponse &rhs) : _statusCode(rhs._statusCode), _header(rhs._header), _body(rhs._body) {}

HttpResponse::~HttpResponse() {}

/*============= oprtr/setr/getr =============*/

HttpResponse &HttpResponse::operator=(const HttpResponse &rhs) {
	if (this != &rhs) {
		this->_statusCode = rhs._statusCode;
		this->_header = rhs._header;
		this->_body = rhs._body;
	}
	return (*this);
}

void	HttpResponse::setStatusCode(int code) {
	if (this->_statusCode != code)
		this->_statusCode = code;
}
void	HttpResponse::addField(const std::string &key, const std::string &value) {
	this->_header[key] = value;
}
void	HttpResponse::setBody(const std::string &body) {
	if (this->_body != body)
		this->_body = body;
}

void	HttpResponse::addContentLength() {
	std::string	ContentLength_str = toStr(_body.size());
	addField("Content-Length", ContentLength_str);
}

void	HttpResponse::setContentType(std::string type) {
	std::string::size_type	find_text = type.find("text/");
	std::string::size_type	find_application = type.find("application/");
	if (find_text == 0 || find_application == 0) {
		std::string::size_type	find_charset = type.find("; charset=utf-8");
		if (find_charset == std::string::npos)
			type += "; charset=utf-8";
	}
	addField("Content-Type", type);
}

bool	HttpResponse::setBodyFromFile(const std::string &filepath) {
	if (filepath.empty() || !fileExist(filepath))
		return false;
	std::ifstream body(filepath.c_str());
	std::stringstream buffer;
	buffer << body.rdbuf();
	this->_body = buffer.str();
	return (true);
}

int	HttpResponse::getStatusCode() {
	return (this->_statusCode);
}

const std::map<std::string, std::string>	&HttpResponse::getHeader() {
	return (_header);
}

const std::string	&HttpResponse::getBody() {
	return (_body);
}

bool	HttpResponse::getField(const std::string &key, std::string& value) {
	std::map<std::string, std::string>::iterator it = _header.find(key);

	if (it != _header.end())
		return false;
	value = it->second;
	return true;
}

std::string	HttpResponse::toHttpString() {
	std::string response("HTTP/1.1 ");
	response += toStr(this->_statusCode) + getReasonPhrase(this->_statusCode);
	response += FieldsToResponse(this->_header);
	response += "\r\n";
	response += this->_body;
	return (response);
}
