#pragma once

#include <HttpMethod.hpp>

class GetMethod : public HttpMethod {
	public:
		GetMethod(ExecutionContext &context);
		~GetMethod();
		void executeMethod(HttpResponse &rsp);
};