//
// Created by sentient_monke on 1/1/1970.
//

#ifndef WEBSERVCONTROLLER_HPP
#define WEBSERVCONTROLLER_HPP

#ifndef MAX_EVENTS
#define MAX_EVENTS 128
#endif

#include <vector>
#include <fstream>
#include <chrono>
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>

#include <ServerConfig.hpp>
#include <Client.hpp>
#include <Socket.hpp>
#include <EpollUtils.hpp>
#include <Server.hpp>

class WebservController {
	private:
		std::vector<ServerConfig>		serverConfigs;
		std::vector<int>				listenFDs;
		std::unordered_map<int, Client>	clients;
		std::vector<Server>				servers;
		int 							epollFD;
		int								eventsWaiting;
//		int								epollSize; not used atm
//		epoll_event						tempEvent;
		epoll_event						eventWaitlist[MAX_EVENTS];
		void	createSockets(int domain, int type, int protocol);
		void	acceptConnection(int listenFd);
		void	errorHandler(const std::runtime_error &err, bool ifExit);
		void	errorLogger(const std::string &errMsg);
		void	cleanResources();
	public:
		WebservController();
		WebservController(const std::string& configFilePath);
		~WebservController();
		void	run();
};



#endif //WEBSERVCONTROLLER_HPP
