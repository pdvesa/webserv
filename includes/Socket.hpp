#ifndef SOCKET_HPP
# define SOCKET_HPP

#include <sys/socket.h>
#include <stdexcept>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include <iostream>
#include <cerrno> 

//idk if we need to save port and host data to identfiy
//maybe add rule of three

class Socket {
	private:
		int			socketFd;
		sockaddr_in	socketAddr;
	public:
		//constructors etc
		Socket(int domain, int type, int protocol, int port, std::string host);
		~Socket();
//		Socket(const Socket &src);
//		Socket &operator=(const Socket &src);
		
		//getters
		int getSocket() const;
		const sockaddr_in &getSocketAddress() const;
		
		//general functionality
		void bindSocket();
		void listenSocket(int backlog);
		void closeSocket();
};

#endif
