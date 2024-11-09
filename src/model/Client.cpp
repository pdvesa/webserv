#include <Client.hpp>

Client::Client(int connection, int listen, ServerConfig conf) : clientFD(connection), listeningSocket(listen), config(conf) {
}

Client::~Client() {
}

int Client::getClientFD() const {
	return (clientFD);
}

int Client::getListening() const {
	return (listeningSocket);
}

ServerConfig &Client::getConfig() {
	return (config);
}


