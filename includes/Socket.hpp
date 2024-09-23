#ifndef SOCKET_HPP
# define SOCKET_HPP

# ifndef PORT_TEST //placeholder
#  define PORT_TEST 8080 
# endif
# ifndef ADDR_TEST //placeholder
#  define ADDR_TEST "127.0.0.1"
# endif

#include<sys/socket.h>
#include<stdexcept>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<cstring>
#include<unistd.h>
#include<iostream>
#include<cerrno>

//maybe need vector of sockets or socketfds
//idk if we need to save port and host data to identfiy
//maybe add rule of three

class Socket {
	private:
		int			socketFd;
		sockaddr_in	sockAdr;
	public:
		//constructor and deconstructor
		Socket(int domain, int type, int protocol, int port, std::string host);
		~Socket();
		
		//getters
		const int getSocket() const;
		const sockaddr_in &getSocketAddress() const;
		
		//general functionality
		void bindSocket();
		void listenSocket(int backlog);
		void acceptConnection(int &connectionFd); // dont know if belongs to here
};

#endif
