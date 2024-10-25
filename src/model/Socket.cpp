#include <Socket.hpp>

Socket::Socket(int domain, int type, int protocol, int port, std::string host) {
	std::cout << port << host << std::endl;
	std::memset(&socketAddr, 0, sizeof(socketAddr));
	socketAddr.sin_family = domain;
	socketAddr.sin_port = htons(port); //parser better validate this sheet and one below
	inet_aton(host.c_str(), &socketAddr.sin_addr); //use getaddrinfo after parsing is done 
	if ((socketFD = socket(domain, type, protocol)) < 0)
		throw std::runtime_error("Failed in socket creation");
	std::cout << "Socket created" << std::endl;
}

Socket::~Socket() {
}

int Socket::getSocket() const {
	return (socketFD);
}

void Socket::bindSocket() {
	if ((bind(socketFD, (sockaddr *)&socketAddr, sizeof(socketAddr))) < 0)
		throw std::runtime_error("Failed in bind socket");
	std::cout << "Socket bound" << std::endl;
}

void Socket::listenSocket(int backlog) {
	if ((listen(socketFD, backlog)) < 0)
		throw std::runtime_error("Failed in listen socket");
	std::cout << "Socket listening to " << inet_ntoa(socketAddr.sin_addr) << ":" << ntohs(socketAddr.sin_port) << std::endl;
}