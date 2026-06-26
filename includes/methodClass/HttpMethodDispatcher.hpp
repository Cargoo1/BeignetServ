#pragma once 

#include <HttpMethod.hpp>

HttpResponse router(const ExecutionContext &context);

/* Helpers */

locationConfig longestMatchingPath(const std::string &path,const serverConfig &server_bloc);
bool checkAllowedMethods(const locationConfig &location_block, const std::string &method);
bool checkClientMaxBodySize(const ExecutionContext &context);
HttpResponse dispatcher_method(const ExecutionContext &context, HttpResponse &rsp);