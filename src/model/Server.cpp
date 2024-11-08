#include <Server.hpp>

Server::Server(ServerConfig data, int fd) : serverData(data), serverFD(fd) {	
}

Server::~Server() {	
}

int Server::getServerFD(){
	return (serverFD);
}

ServerConfig &Server::getServerData() {
	return (serverData);
}