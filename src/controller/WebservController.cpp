//
// Created by sentient_dog on 1/1/1970.
//

#include <WebservController.hpp>

WebservController::WebservController() {	
}

WebservController::~WebservController() {	
}

/*WebservController::WebservController(const std::string& configFilePath) {
	try {
		serverConfigs = ServerConfig::fromConfigFile(configFilePath);
	} catch (...) {
		exit(EXIT_FAILURE);
	}
}*/

void	WebservController::run() {
	/*test variables*/
	std::vector<std::string> addresses = {"127.0.0.1", "127.0.0.2", "127.0.0.3", "127.0.0.4"};
	std::string response;
	response = "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: 5\r\n"
            "\r\n"
            "OKMEN";
	/*test variables*/
	epollFD = epoll_create1(0);
	if (epollFD == -1)
		throw std::runtime_error("Initializing epoll failed, idk :("); //maybe handling needs change
	createSockets(AF_INET, SOCK_STREAM, 0, 8080, addresses); //make into socket class
	while (true) {
		eventsWaiting = epoll_wait(epollFD, eventWaitlist, MAX_EVENTS, -1);
		for (int i = 0; i < eventsWaiting; i++) {
			if (std::find(listenFDs.cbegin(), listenFDs.cend(), eventWaitlist[i].data.fd) != std::end(listenFDs)) {
				acceptConnection(eventWaitlist[i].data.fd);
				std::cout << "We connected from socket " << eventWaitlist[i].data.fd << std::endl;
				clients.back().testPrintRequest(); //test
				send(clients.back().getClientFD(), response.c_str(), response.length(), 0); //test
			}
		}
	}
}

void WebservController::createSockets(int domain, int type, int protocol, int port, std::vector<std::string> hosts) {
	try {
		for (auto &host : hosts) {
			listenFDs.push_back(Socket(domain, type, protocol, port, host)
				.bindSocket()
				.listenSocket(FD_SETSIZE)
				.getSocketFD());
			addToEpoll(listenFDs.back(), true); //hmmh
		}
	} catch (const std::runtime_error &err) {
		errorHandler(err);
	}
}

void WebservController::acceptConnection(int listenFD) {
	Client			client;
	int				connectionFD;
	sockaddr_in		clientTemp = client.getClientAddress();
	unsigned int	clientSize = sizeof(clientTemp);
	std::memset(&clientTemp, 0 , clientSize);
	if ((connectionFD = accept(listenFD, (sockaddr *)&clientTemp, &clientSize)) == -1)
		throw std::runtime_error("Failed in accept connection");
	client.setListening(listenFD); // make it do this in constructor :)()()()()
	client.setClientFD(connectionFD);
	clients.push_back(client);
	std::cout << "Connection accepted for fd " << connectionFD << std::endl;
}

void WebservController::addToEpoll(int fd, bool in) {
	epoll_event	temp;
	if (in)
		temp.events = EPOLLIN;
	else 
		temp.events = EPOLLOUT;
	temp.data.fd = fd;
	if (epoll_ctl(epollFD, EPOLL_CTL_ADD, fd, &temp) == -1) //i hope temp gets copied by kernel otherwise we are d00med
		throw std::runtime_error("Adding socket to epoll failed"); //think about error handling monke like sys_err 
	setNonBlocking(fd);
	epollSize++; // maybe we need, could also add the vector sizes, idk
}

void WebservController::setNonBlocking(int fd) {
	int	flags;
	if ((flags = fcntl(fd, F_GETFL)) == -1)
		throw std::runtime_error("FCUNT CAN'T GET THE FLAGS");
	flags = flags | O_NONBLOCK;
	if (fcntl(fd, F_SETFL, flags) == -1)
		throw std::runtime_error("FCUNT CAN'T SET THE FLAGS");
} //loaned straight from stackoverflow

void WebservController::errorHandler(const std::runtime_error &err) {
	errorLogger(err.what());
//	epoll_ctl(EPOLL_CTL_DEL) remember
	cleanResources();
	exit(EXIT_FAILURE); //maybe needs rework
} // make work with other err types monke

void WebservController::errorLogger(const std::string &errMsg) {
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

void WebservController::cleanResources() {
	for (const int &fd : listenFDs)
		close (fd);
}