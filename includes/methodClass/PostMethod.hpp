#pragma once

#include <HttpMethod.hpp>

class PostMethod : public HttpMethod {
	public:
		PostMethod(const ExecutionContext &context);
		~PostMethod();
		void executeMethod(HttpResponse &rsp);
	private:
};