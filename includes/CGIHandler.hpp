#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include <vector>
#include <string>
#include <filesystem>
#include <unistd.h>
#include <sys/wait.h>
#include <HttpRequest.hpp>
#include <Client.hpp>
#include <signal.h>

class CGI {
	private:
		HttpRequest					req;
		std::vector<std::string>	env;
		std::vector<char *>			envp;
		std::string					cgiResponse;
	public:
		CGI(HttpRequest request);
		~CGI();
		
		void fillEnv();
		void runCGI();
		std::string getCGIResponse(){return cgiResponse;}
};

#endif