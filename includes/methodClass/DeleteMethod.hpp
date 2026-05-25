#pragma once

#include <HttpMethod.hpp>

class DeleteMethod : public HttpMethod {
	public:
		DeleteMethod(ExecutionContext &context);
		~DeleteMethod();
		HttpResponse &executeMethod();
	private:
		bool _removeDirectoryRecursive(const std::string &dirPath);
};