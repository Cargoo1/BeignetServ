#pragma once

#include "Request.hpp"
#include <ExecutionContext.hpp>
#include <HttpResponse.hpp>
#include <utils.hpp>

class HttpMethod {
	public: 
		HttpMethod(Request const& r);
		virtual ~HttpMethod(void);

		virtual void executeMethod(HttpResponse &rsp) = 0;
	protected:
		Request const&			_request;
		std::string				_NormalizePath(std::string toNomalize);
		struct stat				_getFileData(const std::string &filePath);
		HttpResponse			_buildErrorResponse(HttpResponse &res, int code, std::string msg);
};
