#pragma once

#include "Request.hpp"
#include <HttpMethod.hpp>

class PostMethod : public HttpMethod {
	public:
		PostMethod(Request const& r);
		~PostMethod();
		void executeMethod(HttpResponse &rsp);
	private:
};
