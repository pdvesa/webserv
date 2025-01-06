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
		Client						&client;
		const HttpRequest			&req;
		std::string					path;
		std::vector<std::string>	env;
		std::vector<char *>			envp;
		int							exitStatus;
		int							cgiResponse;
		int							pollFD;

		void fillEnv();
		void runCGI();
	public:
		CGI(Client &kunde, const HttpRequest &request, int pfd, std::string target);
		~CGI();
		
		int			getCGIResponse(){return cgiResponse;}
		int			getCGIStatus(){return exitStatus;}
};

#endif