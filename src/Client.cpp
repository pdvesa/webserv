#include <Client.hpp>

Client::Client() {
}

Client::Client(int socket, int response) : listeningSocket(socket), responseFD(response) {	
}

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

const std::string &Client::getRequest() const {
	return (request);
}

void Client::setResponseFD(int fd) {
	responseFD = fd;
}

void Client::setListening(int fd) {
	listeningSocket = fd;
}
