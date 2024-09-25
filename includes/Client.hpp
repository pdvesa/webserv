#ifndef CLIENT_HPP
# define CLIENT_HPP

#ifndef BUF_SIZE
# define BUF_SIZE 4200
#endif

#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>

class Client {
	private:
		int			responseFd;
		int			listeningSocket;
		sockaddr_in	clientAddr;
		std::string	request; //might need diffirent datatype
		//responses go somrwhere
	public:
		Client();
		~Client();
		int getResponseFd() const;
		int getListening() const;
		const sockaddr_in &getClientAddress() const;
		const std::string &getRequest() const;
		void setResponseFd(int fd);
		void setListening(int fd);
		void saveRequest(); //errors mabe idk i have no idea what im doing
};

#endif