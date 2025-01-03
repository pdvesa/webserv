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
#include <EpollUtils.hpp>

class CGI {
	private:
		HttpRequest					req;
		std::vector<std::string>	env;
		std::vector<char *>			envp;
		int							exitStatus;
		int							cgiResponse;

		void fillEnv();
		void runCGI();
	public:
		CGI(HttpRequest request);
		~CGI();
		
		int			getCGIResponse(){return cgiResponse;}
		int			getCGIStatus(){return exitStatus;}
};

#endif