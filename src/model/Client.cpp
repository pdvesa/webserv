#include <Client.hpp>

Client::Client(int connection, int listen, std::shared_ptr<ServerConfig> conf) : clientFD(connection), listeningSocket(listen), config(conf), request(config.get()), response(), cgiFD(0) {
}

Client::~Client() {
}

int Client::getClientFD() {
	return (clientFD);
}

int Client::getListening() const {
	return (listeningSocket);
}

const std::shared_ptr<ServerConfig> Client::getConfig() const {
	return (config);
}

HttpRequest &Client::getRequest() {
	return (request);
}

HttpResponse &Client::getResponse() {
	return (response);
}

void	Client::clearClear() {
	//request.reset();
	//response.reset();
}

void Client::setCgiFD(int fd) {
	cgiFD = fd;
}

int Client::getCgiFD() {
	return (cgiFD);
}
