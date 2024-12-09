//
// Created by sentient_dog on 1/1/1970.
//

#include <WebservController.hpp>
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "HandleRequest.hpp"
#include <unordered_map>

bool running = true;

WebservController::WebservController() {	
}

WebservController::~WebservController() {
	cleanResources();
}

WebservController::WebservController(const std::string& configFilePath) {
		serverConfigs = ServerConfig::fromConfigFile(configFilePath);
}

void	WebservController::run() {
	controllerSignals();
	epollFD = epoll_create1(0);
	if (epollFD == -1)
		throw std::runtime_error("Initializing epoll failed, idk :("); //maybe handling needs change
	createSockets(AF_INET, SOCK_STREAM, 0);
	while (running) {
		eventsWaiting = epoll_wait(epollFD, eventWaitlist, MAX_EVENTS, -1);
		for (int i = 0; i < eventsWaiting; i++) {
			int currentFD = eventWaitlist[i].data.fd;
			if (std::find(listenFDs.cbegin(), listenFDs.cend(), currentFD) != std::end(listenFDs)) {
				try {
					acceptConnection(currentFD); 
				}
				catch (const std::runtime_error &e) {
					errorHandler(e, true);
				}
			}
			else if (eventWaitlist[i].events & EPOLLRDHUP) {
				epollDelete(epollFD, currentFD);
				close(currentFD);
				clients.at(currentFD).clearClear();
			}
			else if (eventWaitlist[i].events & EPOLLIN) {
				try {
					clients.at(currentFD).buildRequest();
				}
				catch (const std::runtime_error &e) {
					errorHandler(e, false);
					continue; 
				}
			}
			else if (eventWaitlist[i].events & EPOLLOUT && clients.at(currentFD).getRequest()) {
				try {
					clients.at(currentFD).buildResponse();			
					if (clients.at(currentFD).getResponse()) { 
						write(currentFD, clients.at(currentFD).getResponse()->toString().c_str(), clients.at(currentFD).getResponse()->toString().length());
						epollDelete(epollFD, currentFD);
						close(currentFD);
						clients.at(currentFD).clearClear();
					}
				}
				catch (const std::runtime_error &e) {
					errorHandler(e, false);
					continue;
				}
			}
		}
	}
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
		errorHandler(err, true);
	}
}

void WebservController::acceptConnection(int listenFD) {
	int				connectionFD;
	sockaddr_in		clientTemp;
	unsigned int	clientSize = sizeof(clientTemp);
	std::memset(&clientTemp, 0 , clientSize);
	if ((connectionFD = accept(listenFD, (sockaddr *)&clientTemp, &clientSize)) == -1)
		throw std::runtime_error("Failed in accept connection");
	auto found = std::find_if(servers.cbegin(), servers.cend(),
	[listenFD](const Server &server)
	{ return server.getServerFD() == listenFD; });
	if (found != servers.cend()) {
		ServerConfig config = found->getServerData();
		Client client(connectionFD, listenFD, config);
		clients.insert_or_assign(connectionFD, client);
		epollAdd(epollFD, connectionFD, true);
		std::cout << "Connection accepted for fd " << connectionFD << std::endl;
	}
}

void WebservController::errorHandler(const std::runtime_error &err, bool ifExit) {
	errorLogger(err.what());
	if (ifExit) {
		cleanResources();
		exit(EXIT_FAILURE);
	}
}

void WebservController::errorLogger(const std::string &errMsg) {
	std::ofstream		errorLog;
	const auto			now = std::chrono::system_clock::now();
	const std::time_t	timestamp = std::chrono::system_clock::to_time_t(now);
	std::cerr << "ERROR: " << errMsg << std::endl;
    errorLog.open("error.log", std::ios::app);
    if (errorLog.is_open()) {
        errorLog << std::ctime(&timestamp) << " ERROR: " << errMsg << std::endl;
        errorLog.close();
    } else 
        std::cerr << "ERROR: Could not open error.log, consider total annihilation of computers!" << std::endl;
}

void WebservController::cleanResources() {
	for (const auto &fd : listenFDs) {
		close (fd);
	}
	for (const auto &pair : clients) {
		close(pair.first);
	}
	close(epollFD);
}

static void sigHandler(int signal) {
	if (signal == SIGINT || signal == SIGTERM || signal == SIGKILL || signal == SIGQUIT)
		running = false;
}

void WebservController::controllerSignals() {
	signal(SIGINT, sigHandler);
	signal(SIGTERM, sigHandler);
	signal(SIGQUIT, sigHandler);
}

