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

class WebservController {
	private:
		std::vector<ServerConfig>	serverConfigs;
		std::vector<int>			listenFDs;
		std::vector<Client>			clients; //maybe not needed
		int 						epollFD;
		int							eventsWaiting;
		int							epollSize;
//		epoll_event					tempEvent;
		epoll_event					eventWaitlist[MAX_EVENTS];
		void	createSockets(int domain, int type, int protocol, int port, std::vector<std::string> hosts);
		void	acceptConnection(int listenFd);
		void	addToEpoll(int fd, bool in);
		void	setNonBlocking(int fd);
		void	errorHandler(const std::runtime_error &err);
		void	errorLogger(const std::string &errMsg);
		void	cleanResources();
	public:
		WebservController();
//		WebservController(const std::string& configFilePath);
		~WebservController();
		void	run();
};



#endif //WEBSERVCONTROLLER_HPP
