#pragma once 

#include <HttpMethod.hpp>

HttpResponse router(const Request &request, const serverConfig &server_bloc);

/* Helpers */

locationConfig longestMatchingPath(const std::string path,const serverConfig &server_bloc);
bool checkAllowedMethods(const locationConfig location_block, const std::string method);
bool checkClientMaxBodySize(const Request request, const serverConfig server, const locationConfig &location);
HttpResponse dispatcher_method(const ExecutionContext &context, HttpResponse &rsp);