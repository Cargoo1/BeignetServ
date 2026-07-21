#pragma once

#include "Request.hpp"
#include <HttpMethod.hpp>

class Server;

class GetMethod : public HttpMethod {
	public:
		GetMethod(Request const& r, Server& server);
		~GetMethod();
		void executeMethod(HttpResponse &rsp);
	private:
		Server& _server;
		std::string _generateAutoindex(const std::string &path);
};
