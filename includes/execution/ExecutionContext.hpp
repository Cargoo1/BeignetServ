#pragma once

#include <Request.hpp>
#include <serverConfig.hpp>

typedef struct ExecutionContext
{
	const Request&			request;
	const locationConfig&	location;
	const serverConfig&		server;

	ExecutionContext(const Request &req, const locationConfig &loc, const serverConfig &serv) : request(req), location(loc), server(serv) {};
};
