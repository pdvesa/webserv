#ifndef CGIHANDLER_HPP
#define CGIHANDLER.HPP

#include <vector>
#include <string>
#include <filesystem>
#include <unistd.h>
#include <HttpRequest.hpp>

class CGI {
	private:
		std::vector<std::string>	env;
		std::vector<char *>			envp;
	public:
		CGI();
		~CGI();
		
		void fillEnv(HttpRequest req);
		void runCGI(HttpRequest req);
};

#endif