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
	createSockets(AF_INET, SOCK_STREAM, 0, 8080, addresses);
	while (true) {
		int listenFd = listenFDs[0]; //test
		acceptConnection(listenFd);
		clients.back().testPrintRequest();
		send(clients.back().getClientFD(), response.c_str(), response.length(), 0);
	}
}

void WebservController::createSockets(int domain, int type, int protocol, int port, std::vector<std::string> hosts) {
	try {
		for (auto &host : hosts) {
			Socket newInstance = Socket(domain, type, protocol, port, host);
			newInstance.bindSocket();
			newInstance.listenSocket(FD_SETSIZE);
			listenFDs.push_back((newInstance.getSocket()));
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
	if ((connectionFD = accept(listenFD, (sockaddr *)&clientTemp, &clientSize)) < 0)
		throw std::runtime_error("Failed in accept connection");
	client.setListening(listenFD);
	client.setClientFD(connectionFD);
	clients.push_back(client);
	std::cout << "Connection accepted for fd " << connectionFD << std::endl;
}

void WebservController::errorHandler(const std::runtime_error &err) {
	errorLogger(err.what());
	cleanResources();
	exit(EXIT_FAILURE); //maybe needs rework
}

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