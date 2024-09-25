#include <Controller.hpp>

# ifndef PORT_TEST //placeholder
#  define PORT_TEST 8080 
# endif
# ifndef ADDR_TEST //placeholder
#  define ADDR_TEST "127.0.0.1"
# endif

Controller::Controller() {
}

Controller::~Controller() {
}

void Controller::controlLoop() {
	/*test variables*/
	std::vector<int> portVector = {8080, 8081, 8082, 8083};
	int listenFd;
	std::string response;
	response = "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: 5\r\n"
            "\r\n"
            "OKMEN";
	/*test variables*/
	createSockets(AF_INET, SOCK_STREAM, 0, portVector, ADDR_TEST);
	while (true) {
		listenFd = socketVector.front().getSocket(); //test
		//this will get listening socket fd from epoll maybe we create a instance of client with every epoll match?
		acceptConnection(listenFd);
		clientVector.front().saveRequest();
		send(clientVector.front().getResponseFd(), response.c_str(), response.length(), 0);
		std::cout << inet_ntoa((clientVector.front().getClientAddress().sin_addr)) << std::endl; //test
		std::cout << ntohs((clientVector.front().getClientAddress().sin_port)) << std::endl; //test
		std::cout << clientVector.front().getRequest() << std::endl; //test print
		close(clientVector.front().getResponseFd()); // i guess we can destroy client after answer ? maybe some other container type
	}
}

void Controller::createSockets(int domain, int type, int protocol, std::vector<int> portVector, std::string host) { //idk if we need multiple addresses
	try {
		for (const int &port : portVector) {
			Socket newInstance = Socket(domain, type, protocol, port, host);
			socketVector.push_back((newInstance));
			socketVector.back().bindSocket();
			socketVector.back().listenSocket(FD_SETSIZE);
		}
	} catch (const std::runtime_error &err) {
		errorHandler(err);
	}
}

void Controller::acceptConnection(int listenFd) {
	Client 			client;
	int				connectionFd;
	sockaddr_in		clientLocal;
	unsigned int	clientSize = sizeof(clientLocal);
	std::memset(&clientLocal, 0 , clientSize);
	if ((connectionFd = accept(listenFd, (sockaddr *)&clientLocal, &clientSize)) < 0)
		throw std::runtime_error("Failed in accept connection");
	client.setListening(listenFd);
	client.setResponseFd(connectionFd);
	clientVector.push_back(client);
	std::cout << "Connection accepted" << std::endl;
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
	for (Socket &socket : socketVector)
		socket.closeSocket();
}
