#pragma once

#include <HttpMethod.hpp>

class DeleteMethod : public HttpMethod {
	public:
		DeleteMethod(ExecutionContext &context);
		~DeleteMethod();
		void executeMethod(HttpResponse &rsp);
	private:
		bool _removeDirectoryRecursive(const std::string &dirPath);
};