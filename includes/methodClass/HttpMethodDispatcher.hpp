#pragma once 

#include "HttpResponse.hpp"
#include "Request.hpp"
#include <HttpMethod.hpp>

class Server;

int router(Request const& r, HttpResponse& response, Server& server);

/* Helpers */

locationConfig longestMatchingPath(const std::string &path,const serverConfig &server_bloc);
bool checkAllowedMethods(const locationConfig &location_block, const std::string &method);
bool checkClientMaxBodySize(Request const& r);
int	dispatcher_method(Request const& r, HttpResponse &rsp, Server& server);
