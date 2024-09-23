#include <Socket.hpp>

int main() {
	Socket sock(AF_INET, SOCK_STREAM, 0, PORT_TEST, ADDR_TEST);
	sock.bindSocket();
	sock.listenSocket(10);
	while (1) {

	}
}

Socket::Socket(int domain, int type, int protocol, int port, std::string host) {
	std::memset(&sockAdr, 0, sizeof(sockAdr));
	sockAdr.sin_family = domain;
	sockAdr.sin_port = htons(port); //parser better validate this sheet
	if (inet_aton(host.c_str(), &sockAdr.sin_addr) == 0) // dont know if even remotely legal also option is INADDR_ANY
		throw std::runtime_error("Invalid address");
	if ((socketFd = socket(domain, type, protocol)) < 0)
		throw std::runtime_error("Socket creation failed! Crashing everything, it's like Rome in in 4th century!!!!");
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
		throw std::runtime_error("Binding socket failed");
	std::cout << "Socket bound" << std::endl;
}

void Socket::listenSocket(int backlog) {
	if ((listen(socketFd, backlog)) < 0)
		throw std::runtime_error("Listen error");
	std::cout << "Socket listening to " << inet_ntoa(sockAdr.sin_addr) << ":" << ntohs(sockAdr.sin_port) << std::endl;
}
