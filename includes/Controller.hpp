#ifndef CONTROLLER_HPP
# define CONTROLLER_HPP

#include <Client.hpp>
#include <Socket.hpp>
#include <vector>
#include <fstream>
#include <chrono>
#include <sys/epoll.h>
#include <unistd.h>

class Controller {
	private:
		std::vector<int> listenFDs;
		std::vector<Client> clientVector; //mayeb this is not necessary, we will see
	public:
		Controller();
		~Controller();
		void controlLoop(); //we want parsed obj here
		void createSockets(int domain, int type, int protocol, std::vector<int> portVector, std::string host); //parsed class later, for now shitty test input
		void acceptConnection(int listenFD);
		void errorHandler(const std::runtime_error &err);
		void errorLogger(const std::string &errMsg);
		void cleanResources();
};

#endif
