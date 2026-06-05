#include <HttpMethodDispatcher.hpp>

namespace {
	int scoringPathMatch(const std::string RequestPath, const std::string LocaPath) {
		std::size_t Rfound = RequestPath.find_last_of("/\\");
		std::size_t Lfound = LocaPath.find_last_of("/\\");
		std::istringstream iss(newPath);
		std::string part, current_path = "";

		if (found != std::string::npos) {
			while (std::getline(iss, part, '/')) {
				if (part.empty())
					continue;
				current_path += part;
				current_path += "/";
				}
			}
		}
}

void router(const Request request, const serverConfig server_bloc) {
	try {
		locationConfig location_bloc = longestMatchingPath(request.getHeader().getTargetResource(), server_bloc);
	}
	catch (Request::ErrorRequest& e) {
		
	}
}

locationConfig longestMatchingPath(const std::string path,const serverConfig server_bloc) {
	locationConfig	ret;
	std::string		absoluteLocPath;
	std::string		serverRoot = server_bloc.root();
	int														bestScore;
	int														tmp;
	std::vector<locationConfig>::const_iterator				bestMatch;
	std::vector<locationConfig>::const_iterator it = server_bloc.locationBloc().begin();
	for (; it != server_bloc.locationBloc().end(); it++) {
		if (!it->getRoot().empty())
			absoluteLocPath = it->getRoot() + it->getPath();
		else
			absoluteLocPath = serverRoot + it->getPath();
		
	}
}

bool checkAllowedMethods(const locationConfig location_block) {

}

bool checkClientMaxBodySize(const serverConfig server_bloc, const locationConfig location_block) {

}

void dispatcher_method(const ExecutionContext context) {

}
