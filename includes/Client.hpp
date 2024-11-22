#ifndef CLIENT_HPP
# define CLIENT_HPP

#ifndef BUF_SIZE
# define BUF_SIZE 420
#endif

#include <iostream>
#include <arpa/inet.h>
#include <ServerConfig.hpp>
#include "HttpResponse.hpp"
#include <optional>
class HttpRequest;
class HttpResponse;
class Client {
	private:
		int				clientFD;
		int				listeningSocket;
		ServerConfig	config;
		std::optional<HttpRequest>		request;
		std::optional<HttpResponse>		response;	
	public:
		Client(int connection, int listen, ServerConfig conf);
		~Client();
		int getClientFD();
		int getListening() const;
		HttpRequest getRequest() const;
		HttpResponse getResponse() const;
		const ServerConfig &getConfig() const;
		void		buildRequest();
		void		buildResponse();
};

#endif
