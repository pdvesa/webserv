#include <Controller.hpp>

void Controller::createSockets(int domain, int type, int protocol, std::vector<int> portVector, std::string host) { //idk if we need multiple addresses
	for (const int &port : portVector) {
		Socket newInstance = Socket(domain, type, protocol, port, host);
		socketVector.push_back((newInstance));
		socketVector.back().bindSocket();
		socketVector.back().listenSocket(5); //remember
	}
}

void Controller::acceptConnection(int &connectionFd, int index) { //make sure that connectioFDs still in scope, might need changing, not sure about index either
	sockaddr_in	client; //this will likely need to be moved somewhere else
	unsigned int clientSize = sizeof(client);
	std::memset(&client, 0, clientSize);
	if ((connectionFd = accept(socketVector.at(index).getSocket(), (sockaddr *)&client, &clientSize)) < 0)
		throw std::runtime_error(strerror(errno));
	std::cout << "Connection accepted" << std::endl;
}

Controller::Controller(std::vector<int> portVector)
{
	socketVector.reserve(portVector.size());
}
Controller::~Controller()
{
}
