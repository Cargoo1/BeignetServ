#pragma once

#include <HttpMethod.hpp>

class PostMethod : public HttpMethod {
	public:
		PostMethod(ExecutionContext &context);
		~PostMethod();
		HttpResponse &executeMethod();
	private:
};