#ifndef CONTROLLER_HPP
# define CONTROLLER_HPP

#include <Socket.hpp>
#include <vector> //consolidate headers i guess
#include <fstream>
#include <chrono>

class Controller {
	private:
		std::vector<Socket> socketVector;
	public:
		Controller();
		~Controller();
		void controlLoop(); //we want parsed obj here
		void createSockets(int domain, int type, int protocol, std::vector<int> portVector, std::string host); //parsed class later, for now shitty test input
		void acceptConnection(int &connectionFd, int index);
		void errorHandler(const std::runtime_error &err);
		void errorLogger(const std::string &errMsg);
		void cleanResources();
};

#endif
