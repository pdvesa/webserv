#include <Client.hpp>

Client::Client() {
}

Client::~Client() {
}

int Client::getClientFD() const {
	return (clientFD);
}

int Client::getListening() const {
	return (listeningSocket);
}

const sockaddr_in &Client::getClientAddress() const {
	return (clientAddr);
}

const std::string &Client::getRequest() const {
	return (request);
}

void Client::setClientFD(int fd) {
	clientFD = fd;
}

void Client::setListening(int fd) {
	listeningSocket = fd;
}

void Client::testPrintRequest() {
	int		recBytes;
	char	buffer[BUF_SIZE];
	while ((recBytes = recv(clientFD, buffer, (BUF_SIZE - 1), 0)) > 0) { 
		buffer[recBytes] = '\0';
		request.append(buffer);
		if (recBytes < (BUF_SIZE - 1)) // i dont know if this can cause problems for "streaming" the request from socket
			break;
	} if (recBytes < 0)
		throw std::runtime_error("Failed reading the request");
	std::cout << "Request saved from socket " << listeningSocket << std::endl;
	std::cout << request << std::endl;
}