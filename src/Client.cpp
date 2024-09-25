#include <Client.hpp>

Client::Client() {
}

Client::~Client() {
}

int Client::getResponseFd() const {
	return (responseFd);
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

void Client::setResponseFd(int fd) {
	responseFd = fd;
}

void Client::setListening(int fd) {
	listeningSocket = fd;
}

void Client::saveRequest() {
	int		recBytes;
	char	buffer[BUF_SIZE];
	while ((recBytes = recv(responseFd, buffer, (BUF_SIZE - 1), 0)) > 0) { 
		buffer[recBytes] = '\0';
		request.append(buffer);
		if (recBytes < (BUF_SIZE - 1)) // i dont know if this can cause problems for "streaming" the request from socket
			break;
	}
	if (recBytes < 0)
		throw std::runtime_error("Failed reading the request");
}