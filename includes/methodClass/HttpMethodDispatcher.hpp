#pragma once 

#include <HttpMethod.hpp>

void router(const Request request, const serverConfig server_bloc);

/* Helpers */

locationConfig longestMatchingPath(const std::string path,const serverConfig server_bloc);
bool checkAllowedMethods(const locationConfig location_block, const std::string method);
bool checkClientMaxBodySize(const serverConfig server_bloc, const locationConfig location_block);
void dispatcher_method(const ExecutionContext context);