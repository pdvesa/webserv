//
// Created by jules on 12/09/2024.
//

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

#include <string>
#include <fstream>
#include <list>
#include <sstream>

#include <Route.hpp>
#include <Parsing.hpp>

class Webserv {
	private:
		const std::string				host;
		const int						port;
		const std::list<std::string>	names;
		const std::list<std::string>	errorsPagesPaths;
		const int						maxClientBodySize;
		const std::list<Route>			routes;

		Webserv();
		Webserv(const std::string& host, int port, const std::string names[],
			const std::string errorsPagesPaths[], int maxClientBodySize, const Route routes[]);
		Webserv(const Webserv& other);

		Webserv& operator=(const Webserv& other);

	public:
		static Webserv	fromConfigFile(const std::string&);

};



#endif
