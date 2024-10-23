#include <Controller.hpp>
#include <algorithm>

# ifndef PORT_TEST //placeholder
#  define PORT_TEST 8080 
# endif
# ifndef ADDR_TEST //placeholder
#  define ADDR_TEST "127.0.0.1"
# endif
#ifndef MAX_EVENTS
# define MAX_EVENTS 42
#endif
#ifndef CONNECTIONS
# define CONNECTIONS 4
#endif

Controller::Controller() {
}

Controller::~Controller() {
}

void Controller::controlLoop() {
	/*test variables*/
	std::vector<int> portVector = {8080, 8081, 8082, 8083};
	std::string response;
	response = "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: 5\r\n"
            "\r\n"
            "OKMEN";
	/*test variables*/
	int epollFd = epoll_create1(0);
	int eventsWaiting;
	epoll_event event[CONNECTIONS]; //we will get these from parser
	epoll_event eventQue[MAX_EVENTS];
	createSockets(AF_INET, SOCK_STREAM, 0, portVector, ADDR_TEST);
	for (int i = 0; i < CONNECTIONS; i++) {
		event[i].events = EPOLLIN;
		event[i].data.fd = listenFDs[i];
		epoll_ctl(epollFd, EPOLL_CTL_ADD, listenFDs[i], &event[i]);
	}
	while (true) {
		eventsWaiting = epoll_wait(epollFd, eventQue, MAX_EVENTS, -1);
		for (int j = 0; j < eventsWaiting; j++) {
			if (std::find(listenFDs.cbegin(), listenFDs.cend(), eventQue[j].data.fd) != std::end(listenFDs)) {
				acceptConnection(eventQue[j].data.fd);
				std::cout << "We connected from socket " << eventQue[j].data.fd << std::endl;
			}
		}
		/*
		clientVector.back().saveRequest(); //we could just call HttpRequest and save fd there
		send(clientVector.back().getResponseFd(), response.c_str(), response.length(), 0);
		for (auto &client : clientVector)
			std::cout << client.getRequest() << std::endl; //test print
		*/
	}
}

void Controller::createSockets(int domain, int type, int protocol, std::vector<int> portVector, std::string host) { //idk if we need multiple addresses
	try {
		for (const int &port : portVector) {
			Socket newInstance = Socket(domain, type, protocol, port, host);
			newInstance.bindSocket();
			newInstance.listenSocket(FD_SETSIZE);
			listenFDs.push_back((newInstance.getSocket()));
		}
	} catch (const std::runtime_error &err) {
		errorHandler(err);
	}
}

void Controller::acceptConnection(int listenFD) {
	int				connectionFD;
	sockaddr_in		clientLocal;
	unsigned int	clientSize = sizeof(clientLocal);
	std::memset(&clientLocal, 0 , clientSize);
	if ((connectionFD = accept(listenFD, (sockaddr *)&clientLocal, &clientSize)) < 0)
		throw std::runtime_error("Failed in accept connection");
	Client			client(listenFD, connectionFD);
	clients.push_back(client);
	std::cout << "Connection accepted for fd " << connectionFD << std::endl;
}

void Controller::errorHandler(const std::runtime_error &err) {
	errorLogger(err.what());
	cleanResources();
	exit(EXIT_FAILURE); //maybe needs rework
}

void Controller::errorLogger(const std::string &errMsg) {
	std::ofstream		errorLog;
	const auto			now = std::chrono::system_clock::now();
	const std::time_t	timestamp = std::chrono::system_clock::to_time_t(now);
	std::cerr << "ERROR: " << errMsg << ": "<< strerror(errno) << std::endl;
    errorLog.open("error.log", std::ios::app);
    if (errorLog.is_open()) {
        errorLog << std::ctime(&timestamp) << " ERROR: " << strerror(errno) << std::endl;
        errorLog.close();
    } else 
        std::cerr << "ERROR: Could not open error.log, consider total annihilation of computers!" << std::endl;
}

void Controller::cleanResources() {
	for (const int &fd : listenFDs)
		close (fd); //close epoll list awaiting client fds also?
}
