//
// Created by jules on 12/09/2024.
//

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

#include <Route.hpp>
#include <string>

class Webserv {
	private:
		const std::string	host;
		const int			port;
		const std::string	names[];
		const std::string	errorsPagesPaths[];
		const int			maxClientBodySize;
		const Route			routes[];
};



#endif
