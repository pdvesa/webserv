#ifndef CLIENT_HPP
# define CLIENT_HPP

#ifndef BUF_SIZE
# define BUF_SIZE 4200
#endif

#include <iostream>
#include <arpa/inet.h>
#include "HttpRequest.hpp"

class Client {
	private:
		int			responseFd;
		int			listeningSocket;
		sockaddr_in	clientAddr;
		HttpRequest	req_class;
//		HttpResponse	response;
		std::string	request;
	public:
		Client();
		~Client();
		int getResponseFd() const;
		int getListening() const;
		const sockaddr_in &getClientAddress() const;
		const std::string &getRequest() const; //placeholder
		void setResponseFd(int fd);
		void setListening(int fd);
		void saveRequest(); //placeholder
};

#endif
