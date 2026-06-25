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

HttpResponse router(const Request &request, const serverConfig &server_bloc) {
	locationConfig location_bloc;
	HttpResponse rsp;
	try {
		location_bloc = server_bloc.locations().at(0)/*longestMatchingPath(request.getHeader().getTargetResource(), server_bloc)*/;
		if (!checkAllowedMethods(location_bloc, request.getHeader().getMethod()))
			throw Request::ErrorRequest(method_not_allowed);
		if (!checkClientMaxBodySize(request, server_bloc, location_bloc))
			throw Request::ErrorRequest(payload_too_large);
	}
	catch (const Request::ErrorRequest& e) {
		rsp.setStatusCode(e.getErrorCode());
		std::cerr << e.what() << '\n';
		return (rsp);
	}
	ExecutionContext execCont(request, location_bloc, server_bloc);
	dispatcher_method(execCont, rsp);
	return (rsp);
}

locationConfig longestMatchingPath(const std::string path,const serverConfig &server_bloc) {
	locationConfig										ret;
	std::string											absoluteLocPath;
	std::vector<std::string>							split_reqPath = splitPath(path), split_locPath;
	std::string											serverRoot = server_bloc.root();
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
		throw Request::ErrorRequest(not_found);
	return (bestMatch);
}

bool checkAllowedMethods(const locationConfig location_block, const std::string method) {
	std::vector<std::string>					loc_method = location_block.getMethod();
	std::vector<std::string>::const_iterator	it;

	for (it = loc_method.begin(); it != loc_method.end(); it++) {
		if (*it == method)
			return (true);
	}
	return (false);
}

bool checkClientMaxBodySize(const Request request, const serverConfig server, const locationConfig &location) {
	if (request.getHeader().getFields().find("Content-Length") == request.getHeader().getFields().end())
		return (true);
	std::size_t clientMBS;
	std::size_t contentLenght;
	toDigit(request.getHeader().getFields().find("Content-Length")->second, contentLenght);
	if (location.hasCMBS())
		clientMBS = location.getCMBS();
	else
		clientMBS = server.getCMBS();
	if (contentLenght > clientMBS)
		return (false);
	return (true);
}

HttpResponse dispatcher_method(const ExecutionContext &context, HttpResponse &rsp) {
	std::string reqMethod(context.request.getHeader().getMethod());

	if (reqMethod == "GET"){
		GetMethod	method(context);
		method.executeMethod(rsp);
	}
	else if (reqMethod == "DELETE") {
		DeleteMethod	method(context);
		method.executeMethod(rsp);
	}
	else if (reqMethod == "POST") {
		PostMethod	method(context);
		method.executeMethod(rsp);
	}
	return (rsp);
}
