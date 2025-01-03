//
// Created by sentient_dog on 1/1/1970.
//

#include <WebservController.hpp>

bool running = true;

WebservController::WebservController()
{ }

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
		throw std::runtime_error("Initializing epoll failed, idk :("); //we dont catch
	createSockets(AF_INET, SOCK_STREAM, 0);
	while (running) {
		eventsWaiting = epoll_wait(epollFD, eventWaitlist, MAX_EVENTS, -1);
		for (int i = 0; i < eventsWaiting; i++) {
			int currentFD = eventWaitlist[i].data.fd;
			if (std::find(listenFDs.cbegin(), listenFDs.cend(), currentFD) != std::end(listenFDs))
				acceptConnection(currentFD); 
			else if (eventWaitlist[i].events & EPOLLRDHUP) {
				epollDelete(epollFD, currentFD);
				close(currentFD);
				clients.at(currentFD).clearClear(); //change also clear the map values
				std::cout << "DEBUG" << std::endl;
			}
			else if (eventWaitlist[i].events & EPOLLIN)
				makeRequest(currentFD);
			else if (eventWaitlist[i].events & EPOLLOUT)
				makeResponse(currentFD);
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
	if ((connectionFD = accept(listenFD, (sockaddr *)&clientTemp, &clientSize)) == -1) {
		errorLogger("Syscall 'accept' failed because: " + std::string(strerror(errno)));
		return ; 
	}
	auto found = std::find_if(servers.cbegin(), servers.cend(),
	[listenFD](const Server &server)
	{ return server.getServerFD() == listenFD; });
	if (found != servers.cend()) {
		std::shared_ptr<ServerConfig> config = found->getServerPtr();
		Client client(connectionFD, listenFD, config);
		clients.insert_or_assign(connectionFD, client);
		epollAdd(epollFD, connectionFD, true);
	}
}

void WebservController::makeRequest(int fd) {
	std::vector<unsigned char>	buffer(BUF_SIZE);
	HttpRequest					&req = clients.at(fd).getRequest();
	int							rb;
	if (req.getRequestState() == REQUEST_PARSING) {
		if ((rb = read(fd, buffer.data(), BUF_SIZE)) > 0) {
			buffer.resize(rb);
			for (u_char &c : buffer)
				std::cout << c;
			req.parseData(buffer.data(), rb);
		} 
		if (rb == 0)
			std::cerr << "Something" << std::endl;
		if (rb == -1)
			std::cerr << "Failure of everything" << std::endl;
	}
	else
		exit(0);
}

void WebservController::makeResponse(int fd) {
	HttpRequest					&req = clients.at(fd).getRequest();
	if (req.getRequestState() != REQUEST_PARSING && req.getRequestState() != REQUEST_CHUNK_RECEIVING) {
		RequestHandler				handler(req);
		handler.handle();
		HttpResponse				response = handler.buildResponse();
		std::vector<unsigned char>	rVector = response.asResponseBuffer();
		int wb = write(fd, rVector.data(), rVector.size()); // NOLINT(*-narrowing-conversions)
		epollDelete(epollFD, fd);
		close(fd);
		clients.erase(fd);
		if (wb == -1)
			std::cerr << "Something" << std::endl;
		else if (wb == 0)
			std::cerr << "Something something" << std::endl;
	}
}

void WebservController::errorHandler(const std::runtime_error &err, bool ifExit) {
	errorLogger(err.what());
	if (ifExit)
		running = false;
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
	if (signal == SIGINT || signal == SIGTERM || signal == SIGQUIT)
		running = false;
}

void WebservController::controllerSignals() {
	signal(SIGINT, sigHandler);
	signal(SIGTERM, sigHandler);
	signal(SIGQUIT, sigHandler);
}
