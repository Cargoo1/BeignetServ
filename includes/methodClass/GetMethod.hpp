#pragma once

#include "Request.hpp"
#include <HttpMethod.hpp>

class GetMethod : public HttpMethod {
	public:
		GetMethod(Request const& r);
		~GetMethod();
		void executeMethod(HttpResponse &rsp);
	private:
		std::string _generateAutoindex(const std::string &path);
};
