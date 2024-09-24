#ifndef CONTROLLER_HPP
# define CONTROLLER_HPP

#include <Socket.hpp>
#include <vector> //consolidate headers i guess

class Controller {
	private:
		std::vector<Socket *> socketVector; //eh
	public:
		void createSockets(int domain, int type, int protocol, std::vector<int> portVector, std::string host); //parsed class later, for now shitty test input
		void acceptConnection(int &connectionFd, int index);
};

#endif