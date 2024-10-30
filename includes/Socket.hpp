#ifndef SOCKET_HPP
# define SOCKET_HPP

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <cerrno> 


class Socket {
	private:
		int			socketFD;
		sockaddr_in	socketAddr;
	public:
		Socket(int domain, int type, int protocol, int port, std::string host);
		~Socket();
		Socket&		bindSocket();
		Socket&		listenSocket(int backlog);
		int			getSocketFD();
};

#endif
