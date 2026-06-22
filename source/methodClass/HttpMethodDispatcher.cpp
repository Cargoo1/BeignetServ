#include <HttpMethodDispatcher.hpp>

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

void router(const Request request, const serverConfig server_bloc) {
	try {
		locationConfig location_bloc = longestMatchingPath(request.getHeader().getTargetResource(), server_bloc);
		if (!checkAllowedMethods(location_bloc, request.getHeader().getMethod()))
			throw Request::ErrorRequest(method_not_allowed);
	}
	catch (const Request::ErrorRequest& e) {
		
	}
}

locationConfig longestMatchingPath(const std::string path,const serverConfig server_bloc) {
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

bool checkClientMaxBodySize(const serverConfig server_bloc, const locationConfig location_block) {
	std::size_t clientMBS;
	if (location_block.hasCMBS())
		clientMBS = location_block.getCMBS();
	else
		clientMBS = server_bloc.getCMBS();

	/*
		CHECKER SI LE CLIENT N'ENVOIE RIEN DE PLUS LOURDS QUE le C.M.B.S
		if (r.haveBody()) {
			if (r.getBodyL > clientMBS)
				throw 413 ou 415
		}
		puis switch sur method qui renvoie une reponse HTTP ou qui la prenne en param
	*/

}

void dispatcher_method(const ExecutionContext context) {
	// Switch ou if dispath on r.getmethod
		// create the right methd with context
}
