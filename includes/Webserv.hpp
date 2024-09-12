//
// Created by jules on 12/09/2024.
//

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

#include <Route.hpp>
#include <string>

class Webserv {
	private:
		std::string	host;
		int			port;
		std::string	names[];
		std::string	errorsPagesPaths[];
		int			maxClientBodySize;
		Route		routes[];
};



#endif
