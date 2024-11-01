#ifndef CLIENT_HPP
# define CLIENT_HPP

#ifndef BUF_SIZE
# define BUF_SIZE 4200
#endif

#include <iostream>
#include <arpa/inet.h>

class Client {
	private:
		int				clientFD;
		int				listeningSocket;
		sockaddr_in		clientAddr;
//		HttpRequest		request;
//		HttpResponse	response;	
		std::string		request;
	public:
		Client();
		~Client();
		int getClientFD() const;
		int getListening() const;
		const sockaddr_in &getClientAddress() const;
		const std::string &getRequest() const; //placeholder
		void setClientFD(int fd);
		void setListening(int fd);
};

#endif