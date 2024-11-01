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
