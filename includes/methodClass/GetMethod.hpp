#pragma once

#include <HttpMethod.hpp>

class GetMethod : public HttpMethod {
	public:
		GetMethod(const ExecutionContext &context);
		~GetMethod();
		void executeMethod(HttpResponse &rsp);
	private:
		std::string _generateAutoindex(const std::string &path);
};