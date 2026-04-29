#pragma once

#include <location.hpp>
#include <location.hpp>


class WebServ {
	public:
		WebServ();
		WebServ(const WebServ &rhs);
		~WebServ();

		WebServ &operator=(const WebServ &rhs);

		
	private:
			server		_serv;
			location	*_locs;
};