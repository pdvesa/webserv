//
// Created by sentient_monke on 1/1/1970.
//

#ifndef WEBSERVCONTROLLER_HPP
#define WEBSERVCONTROLLER_HPP

#ifndef MAX_EVENTS
#define MAX_EVENTS 128
#endif

#ifndef TIMEOUT
#define TIMEOUT 5
#endif

#include <vector>
#include <fstream>
#include <chrono>
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

#include <ServerConfig.hpp>
#include <Client.hpp>
#include <Socket.hpp>
#include <EpollUtils.hpp>
#include <Server.hpp>
#include <unordered_map>
#include <RequestHandler.hpp>
#include <HttpRequest.hpp>
#include <HttpResponse.hpp>

class RequestHandler;

class WebservController {
	private:
		std::vector<ServerConfig>		serverConfigs;
		std::vector<int>				listenFDs;
		std::unordered_map<int, Client>	clients;
		std::vector<Server>				servers;
		int 							epollFD;
		int								cgiPoll;
		int								eventsWaiting;
		int								cgiEvents;
		epoll_event						eventWaitlist[MAX_EVENTS];
		epoll_event						cgiEventWaitlist[MAX_EVENTS];
		void	createSockets(int domain, int type, int protocol);
		void	acceptConnection(int listenFd);
		void	errorHandler(const std::runtime_error &err, bool ifExit);
		void	errorLogger(const std::string &errMsg);
		void	cleanResources();
		static void	controllerSignals();
		void	handleCGIClient(int fd, bool closed);
		void	makeResponse(int fd);
		void	makeRequest(int fd);
		void	checkForTimeout();
	public:
		WebservController();
		WebservController(const std::string& configFilePath);
		~WebservController();
		void	run();
};



#endif //WEBSERVCONTROLLER_HPP
