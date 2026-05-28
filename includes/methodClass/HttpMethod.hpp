#pragma once

#include <ExecutionContext.hpp>
#include <HttpResponse.hpp>

class HttpMethod {
	public: 
		HttpMethod(const ExecutionContext &context);
		virtual ~HttpMethod(void);

		virtual void executeMethod(HttpResponse &rsp) = 0;
	protected:
		const ExecutionContext &_context;
		std::string NormalizePath(std::string toNomalize);
		struct stat getFileData(const std::string &filePath);
		std::string getContentType(const std::string &filepath);
		HttpResponse buildErrorResponse(HttpResponse &res, int code, std::string msg);
};