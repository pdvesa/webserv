#include <Socket.hpp>

int main() {
	Socket sock(AF_INET, SOCK_STREAM, 0, PORT_TEST, ADDR_TEST);
	int fd;
	int rb;
	char buffer[10000];
	sock.bindSocket();
	sock.listenSocket(10);
	while (1) {
		sock.acceptConnection(fd);
		rb = read(fd, buffer, 10000);
		std::cout << fd <<std::endl;
		if (rb > 0)
			std::cout << buffer << std::endl;
		close (fd);
	}
}

Socket::Socket(int domain, int type, int protocol, int port, std::string host) {
	std::memset(&sockAdr, 0, sizeof(sockAdr));
	sockAdr.sin_family = domain;
	sockAdr.sin_port = htons(port); //parser better validate this sheet and one below
	inet_aton(host.c_str(), &sockAdr.sin_addr); //use getaddrinfo after parsing is done 
	if ((socketFd = socket(domain, type, protocol)) < 0)
		throw std::runtime_error(strerror(errno));
	std::cout << "Socket created" << std::endl;
}

Socket::~Socket() {
	close (socketFd); //no idea if needed
}

const int Socket::getSocket() const {
	return (socketFd);
}

const sockaddr_in &Socket::getSocketAddress() const {
	return (sockAdr);
}

void Socket::bindSocket() {
	if ((bind(socketFd, (sockaddr *)&sockAdr, sizeof(sockAdr))) < 0)
		throw std::runtime_error(strerror(errno));
	std::cout << "Socket bound" << std::endl;
}

void Socket::listenSocket(int backlog) {
	if ((listen(socketFd, backlog)) < 0)
		throw std::runtime_error(strerror(errno));
	std::cout << "Socket listening to " << inet_ntoa(sockAdr.sin_addr) << ":" << ntohs(sockAdr.sin_port) << std::endl;
}

void Socket::acceptConnection(int &connectionFd) { //make sure that connectioFDs still in scope, might need changing
	sockaddr_in	client; //this will likely need to be moved somewhere else
	socklen_t clientSize = sizeof(client);
	std::memset(&client, 0 , sizeof(client));
	if ((connectionFd = accept(socketFd, (sockaddr *)&client, &clientSize)) < 0)
		throw std::runtime_error(strerror(errno));
	std::cout << "Connection accepted" << std::endl;
}
