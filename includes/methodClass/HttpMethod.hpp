#pragma once

#include <ExecutionContext.hpp>
#include <HttpResponse.hpp>
#include <utils.hpp>

class HttpMethod {
	public: 
		HttpMethod(const ExecutionContext &context);
		virtual ~HttpMethod(void);

		virtual void executeMethod(HttpResponse &rsp) = 0;
	protected:
		const ExecutionContext	&_context;
		std::string				_NormalizePath(std::string toNomalize);
		struct stat				_getFileData(const std::string &filePath);
		HttpResponse			_buildErrorResponse(HttpResponse &res, int code, std::string msg);
		void					_executeCGI(const ExecutionContext context);
};
