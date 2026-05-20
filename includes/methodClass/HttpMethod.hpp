#pragma once

#include <ExecutionContext.hpp>
#include <HttpResponse.hpp>

class HttpMethod {
	public: 
		HttpMethod(const ExecutionContext &context);
		virtual ~HttpMethod(void);

		virtual void executeMethod() = 0;
	protected:
		const ExecutionContext &_context;

		bool verifyPath();
		bool verifyContentLength();
		bool verifyBodySize();
		bool checkAllowedMethods();
		bool checkResourceExists();
		bool checkFilePermissions();
		std::string resolveResourcePath();
		std::string getContentType(const std::string &filepath);
		void buildErrorResponse(HttpResponse &res, int code, std::string msg);
};