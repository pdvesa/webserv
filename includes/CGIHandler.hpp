#ifndef CGIHANDLER_HPP
#define CGIHANDLER.HPP

#include <vector>
#include <string>
#include <filesystem>
#include <unistd.h>
#include <sys/wait.h>
#include <HttpRequest.hpp>
#include <Client.hpp>

class CGI {
	private:
		std::vector<std::string>	env;
		std::vector<char *>			envp;
		std::string					cgiResponse;
	public:
		CGI();
		~CGI();
		
		void fillEnv(HttpRequest req);
		void runCGI(HttpRequest req);
};

#endif