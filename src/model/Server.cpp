#include <Server.hpp>

Server::Server(ServerConfig data, int fd) : serverData(data), configPtr(std::make_shared<ServerConfig>(data)), serverFD(fd) {
}

Server::~Server() {	
}

int Server::getServerFD() const {
	return (serverFD);
}

ServerConfig Server::getServerData() const {
	return (serverData);
}

std::shared_ptr<ServerConfig> Server::getServerPtr() const {
    return (configPtr);
}