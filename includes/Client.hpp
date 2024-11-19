#ifndef CLIENT_HPP
# define CLIENT_HPP

#ifndef BUF_SIZE
# define BUF_SIZE 4200
#endif

#include <iostream>
#include <arpa/inet.h>
#include <ServerConfig.hpp>

class Client {
	private:
		int				clientFD;
		int				listeningSocket;
		ServerConfig	config;
//		HttpRequest		request;
//		HttpResponse	response;	
	public:
		Client(int connection, int listen, ServerConfig conf);
		~Client();
		int getClientFD() const;
		int getListening() const;
		const ServerConfig &getConfig() const;
};

#endif
