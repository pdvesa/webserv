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
	int		readBytes;
	char	buffer[BUF_SIZE];
	/*while ((readBytes = read(responseFd, buffer, BUF_SIZE - 1)) > 0) { for some reason we never read 0 IDK maybe brain damagu
		std::cout << readBytes << std::endl;
		buffer[readBytes] = '\0';
		request.append(buffer);
	}*/
	readBytes = read(responseFd, buffer, BUF_SIZE);
	buffer[readBytes] = '\0';
	request.append(buffer);
	if (readBytes < 0)
		throw std::runtime_error("Failed reading the request");
}