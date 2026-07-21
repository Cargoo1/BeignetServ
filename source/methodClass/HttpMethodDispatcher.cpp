#include "Header.hpp"
#include "HttpResponse.hpp"
#include "Request.hpp"
#include "locationConfig.hpp"
#include "serverConfig.hpp"
#include "utils.hpp"
#include <HttpMethodDispatcher.hpp>
#include <GetMethod.hpp>
#include <PostMethod.hpp>
#include <DeleteMethod.hpp>
#include <iostream>

#define NOT_FOUND -1

namespace {
	int scoringPathMatch(const std::vector<std::string> reqPath, const std::vector<std::string> locPath) {
		std::vector<std::string>::const_iterator	r_it	= reqPath.begin(),			l_it	= locPath.begin();
		int ret = 0, min = 3;
		while (*r_it == *l_it)
		{
			ret++;
			if (r_it != reqPath.end())
				r_it++;
			else
				break;
			if (l_it != locPath.end())
				l_it++;
			else
				break;
		}
		if (ret <= min)
			ret = NOT_FOUND;
		return (ret);
	}
}

int router(Request const& r, HttpResponse& response, Server& server) {
	locationConfig const&	location_block = *r.getLocConfBlock();
	int		infno = 0;

	if (!checkAllowedMethods(location_block, r.getHeader().getMethod()))
	{
		response.setStatusCode(method_not_allowed);
		return -1;
	}
	if (!checkClientMaxBodySize(r))
	{
		response.setStatusCode(payload_too_large);
		return -1;
	}
	try
	{
		infno = dispatcher_method(r, response, server);
	}
	catch (Request::ErrorRequest& e)
	{
		response.setStatusCode(e.getErrorCode());
		return -1;
	}
	return (infno);
}

locationConfig longestMatchingPath(const std::string &path,const serverConfig &server_bloc) {
	locationConfig										ret;
	std::string											absoluteLocPath;
	std::vector<std::string>							split_reqPath = splitPath(path), split_locPath;
	std::string											serverRoot = server_bloc.getRoot();
	int													bestScore = NOT_FOUND, tmp;
	locationConfig										bestMatch;
	std::vector<locationConfig>::const_iterator it =	server_bloc.locations().begin();

	for (; it != server_bloc.locations().end(); it++) {
		if (!it->getRoot().empty())
			absoluteLocPath = it->getRoot() + it->getPath();
		else
			absoluteLocPath = serverRoot + it->getPath();
		split_locPath = splitPath(absoluteLocPath);
		tmp = scoringPathMatch(split_reqPath, split_locPath);
		if (tmp > bestScore) {
			bestScore = tmp;
			bestMatch = *it;
		}
	}
	if (bestScore == NOT_FOUND)
		throw Request::ErrorRequest(not_found, "Path not found");
	return (bestMatch);
}

bool checkAllowedMethods(const locationConfig &location_block, const std::string &method) {
	std::vector<std::string>					loc_method = location_block.getMethod();
	std::vector<std::string>::const_iterator	it;

	for (it = loc_method.begin(); it != loc_method.end(); it++) {
		if (*it == method)
			return (true);
	}
	return (false);
}

bool checkClientMaxBodySize(Request const& r) {
	Header const&	header = r.getHeader();
	if (header.getFields().find("Content-Length") == header.getFields().end())
		return (true);
	std::size_t clientMBS;
	std::size_t contentLenght;
	toDigit(header.getFields().find("Content-Length")->second, contentLenght);
	if (r.getLocConfBlock()->hasCMBS())
		clientMBS = r.getLocConfBlock()->getCMBS();
	else
		clientMBS = r.getServerBlock()->getCMBS();
	if (contentLenght > clientMBS)
		return (false);
	return (true);
}

int dispatcher_method(Request const& r, HttpResponse &response, Server& server) {
	std::string reqMethod(r.getHeader().getMethod());
	if (r.getHeader().is_a_script())
		return 2;
	else if (reqMethod == "GET"){
		GetMethod	method(r, server);
		method.executeMethod(response);
	}
	else if (reqMethod == "DELETE") {
		DeleteMethod	method(r);
		method.executeMethod(response);
	}
	else if (reqMethod == "POST") {
		PostMethod	method(r);
		method.executeMethod(response);
	}
	return 0;
}
