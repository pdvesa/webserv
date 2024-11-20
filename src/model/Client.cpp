#include <Client.hpp>

Client::Client(int connection, int listen, ServerConfig conf) : clientFD(connection), listeningSocket(listen), config(conf), request(), response() {
}

Client::~Client() {
}

int Client::getClientFD() {
	return (clientFD);
}

int Client::getListening() const {
	return (listeningSocket);
}

const ServerConfig &Client::getConfig() const {
	return (config);
}

void	Client::buildRequest()
{
	request.emplace(HttpRequest(getConfig(), getClientFD()));
}
