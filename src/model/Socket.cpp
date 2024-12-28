#include <Socket.hpp>

Socket::Socket(int domain, int type, int protocol, int port, std::string host) {
	int			status;
	addrinfo	hints;
	addrinfo	*result;
	std::memset(&socketAddr, 0, sizeof(socketAddr));
	std::memset(&hints, 0, sizeof(hints));
	socketAddr.sin_family = domain;
	socketAddr.sin_port = htons(port);
    hints.ai_family = domain;
    hints.ai_socktype = type;
    status = getaddrinfo(host.c_str(), nullptr, &hints, &result);
    if (status) {
		throw std::runtime_error("Failed in resolving the address: " + std::string(gai_strerror(status)));
		freeaddrinfo(result);
	}
	socketAddr.sin_addr = ((sockaddr_in *)result->ai_addr)->sin_addr;
	freeaddrinfo(result);
	if ((socketFD = socket(domain, type, protocol)) == -1)
		throw std::runtime_error("Failed in socket creation: " + std::string(strerror(errno)));
	std::cout << "Socket created" << std::endl;
}

Socket::~Socket() {
}

Socket &Socket::bindSocket() {
	int sock_opts = 1;
	if (setsockopt(socketFD, SOL_SOCKET, SO_REUSEADDR, &sock_opts, sizeof(int)) < 0)
		throw std::runtime_error ("Failed to set socket options");
	if ((bind(socketFD, (sockaddr *)&socketAddr, sizeof(socketAddr))) == -1)
		throw std::runtime_error("Failed in bind socket: " + std::string(strerror(errno)));
	std::cout << "Socket bound" << std::endl;
	return (*this);
}

Socket &Socket::listenSocket(int backlog) {
	if ((listen(socketFD, backlog)) == -1)
		throw std::runtime_error("Failed in listen socket");
	std::cout << "Socket listening to " << inet_ntoa(socketAddr.sin_addr) << ":" << ntohs(socketAddr.sin_port) << std::endl;
	return (*this);
}

int Socket::getSocketFD() {
	return (socketFD);
}

