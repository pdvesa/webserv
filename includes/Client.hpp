#ifndef CLIENT_HPP
# define CLIENT_HPP

#ifndef BUF_SIZE
# define BUF_SIZE 1
#endif

#include <memory>
#include <iostream>
#include <arpa/inet.h>
#include <ServerConfig.hpp>
#include <optional>
#include <HttpRequest.hpp>
#include <HttpResponse.hpp>

class HttpRequest;
class HttpResponse;

class Client {
	private:
		int				clientFD;
		int				listeningSocket;
		std::shared_ptr<ServerConfig>	config;
		HttpRequest		request;
		HttpResponse	response;
		int 			cgiFD;
	public:
		Client(int connection, int listen, std::shared_ptr<ServerConfig> conf);
		~Client();
		int getClientFD();
		int getListening() const;
		HttpRequest &getRequest();
		HttpResponse &getResponse();
		const std::shared_ptr<ServerConfig> getConfig() const;
		void		clearClear();
		void		setCgiFD(int fd);
		int 		getCgiFD();
};

#endif
