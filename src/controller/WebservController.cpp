//
// Created by sentient_dog on 1/1/1970.
//

#include <WebservController.hpp>

WebservController::WebservController() {	
}

WebservController::~WebservController() {	
}

WebservController::WebservController(const std::string& configFilePath) {
		serverConfigs = ServerConfig::fromConfigFile(configFilePath);
}

void	WebservController::run() {
	/*test variables*/
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
	createSockets(AF_INET, SOCK_STREAM, 0); //make into socket class
	while (true) {
		eventsWaiting = epoll_wait(epollFD, eventWaitlist, MAX_EVENTS, -1);
		for (int i = 0; i < eventsWaiting; i++) {
			int currentFD = eventWaitlist[i].data.fd;
			if (std::find(listenFDs.cbegin(), listenFDs.cend(), currentFD) != std::end(listenFDs)) {
				acceptConnection(currentFD); 
				std::cout << "We connected from socket " << currentFD << std::endl;
			} else if (eventWaitlist[i].events & EPOLLIN) {
				testPrintRequest(currentFD);
				auto found = std::find_if(clients.cbegin(), clients.cend(),
					[currentFD](const Client &client)
					{ return client.getClientFD() == currentFD; });
				if (found != clients.cend()) 
        			epollModify(epollFD, found->getClientFD());
			} else if (eventWaitlist[i].events & EPOLLOUT) {
				send(currentFD, response.c_str(), response.length(), 0); //test
				epollDelete(epollFD, currentFD);
				close(currentFD);
			}
		}
	}
}

void WebservController::testPrintRequest(int fd) {
	int		recBytes;
	char	buffer[BUF_SIZE];
	while ((recBytes = recv(fd, buffer, (BUF_SIZE - 1), 0)) > 0) { 
		buffer[recBytes] = '\0';
		request.append(buffer);
		if (recBytes < (BUF_SIZE - 1))
			break;
	} if (recBytes < 0)
		throw std::runtime_error("Failed reading the request");
	std::cout << request << std::endl;
}

void WebservController::createSockets(int domain, int type, int protocol) {
	try {
		for (auto &server : serverConfigs) {
			listenFDs.push_back(Socket(domain, type, protocol, server.getPort(), server.getHost())
				.bindSocket()
				.listenSocket(FD_SETSIZE)
				.getSocketFD());
			servers.push_back(Server(server, listenFDs.back()));
			epollAdd(epollFD, listenFDs.back(), true); 
		}
	} catch (const std::runtime_error &err) {
		errorHandler(err);
	}
}

void WebservController::acceptConnection(int listenFD) {
	Client			client;
	int				connectionFD;
	sockaddr_in		clientTemp;
	unsigned int	clientSize = sizeof(clientTemp);
	std::memset(&clientTemp, 0 , clientSize);
	if ((connectionFD = accept(listenFD, (sockaddr *)&clientTemp, &clientSize)) == -1)
		throw std::runtime_error("Failed in accept connection");
	client.setListening(listenFD); // make it do this in constructor :)()()()()
	client.setClientFD(connectionFD);
	clients.push_back(client); //if needed save addr
	epollAdd(epollFD, connectionFD, true);
	std::cout << "Connection accepted for fd " << connectionFD << std::endl;
}

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