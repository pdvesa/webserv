#include <Server.hpp>

Server::Server(ServerConfig data, int fd) : serverData(data), serverFD(fd) {	
}

Server::~Server() {	
}

int Server::getServerFD() const {
	return (serverFD);
}

ServerConfig Server::getServerData() const {
	return (serverData);
}