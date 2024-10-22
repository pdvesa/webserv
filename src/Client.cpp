#include <Client.hpp>

Client::Client(int socket) {
	request = HttpRequest(socket);
} //fix

Client::~Client() {
}

int Client::getResponseFD() const {
	return (responseFD);
}

int Client::getListening() const {
	return (listeningSocket);
}

const sockaddr_in &Client::getClientAddress() const {
	return (clientAddr);
}

const HttpRequest &Client::getRequest() const {
	return (request);
}

void Client::setResponseFD(int fd) {
	responseFD = fd;
}

void Client::setListening(int fd) {
	listeningSocket = fd;
}

void Client::setRequest(int socket) {
	request = HttpRequest(socket);
}
