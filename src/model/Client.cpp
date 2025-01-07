#include <Client.hpp>

Client::Client(int connection, int listen, std::shared_ptr<ServerConfig> conf) : clientFD(connection), listeningSocket(listen), config(conf), request(config.get()), response(), cgiState(NO_CGI), pid(0), cgiResp(), timestamp(std::chrono::steady_clock::now()) {
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

void Client::setCgiFD(int fd) {
	cgiFD = fd;
}

int Client::getCgiFD() {
	return (cgiFD);
}

void Client::setCgiStatus(int status) {
	cgiState = status;
}

int Client::getCgiStatus() {
	return (cgiState);
}

pid_t Client::getPid() {
	return (pid);
}

void Client::setPid(pid_t p) {
	pid = p;
}

std::vector<unsigned char> &Client::getCgiResponse() {
	return (cgiResp);
}

std::chrono::steady_clock::time_point Client::getTimestamp() {
	return (timestamp);
}
