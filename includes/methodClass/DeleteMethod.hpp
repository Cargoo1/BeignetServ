#pragma once

#include "Request.hpp"
#include <HttpMethod.hpp>

class DeleteMethod : public HttpMethod {
	public:
		DeleteMethod(Request const& r);
		~DeleteMethod();
		void executeMethod(HttpResponse &rsp);
	private:
		bool _removeDirectoryRecursive(const std::string &dirPath);
};
