#ifndef CLIENT_HPP
# define CLIENT_HPP

#ifndef BUF_SIZE
# define BUF_SIZE 42
#endif

#include <iostream>
#include <arpa/inet.h>
#include "HttpRequest.hpp"

class Client {
	private:
		int			responseFD;
		int			listeningSocket;
		sockaddr_in	clientAddr;
		HttpRequest	request;
//		HttpResponse	response;
	public:
		Client(int socket);
		~Client();
		int getResponseFD() const;
		int getListening() const;
		const sockaddr_in &getClientAddress() const;
		const HttpRequest &getRequest() const; //placeholder
		void setResponseFD(int fd);
		void setListening(int fd);
		void setRequest(int socket);
};

#endif
