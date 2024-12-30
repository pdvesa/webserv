#ifndef CLIENT_HPP
# define CLIENT_HPP

#ifndef BUF_SIZE
# define BUF_SIZE 1028
#endif

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
		ServerConfig	config;
		HttpRequest		request;
		std::optional<HttpResponse>	response;
		int 			cgiFD;
	public:
		Client(int connection, int listen, ServerConfig conf);
		~Client();
		int getClientFD();
		int getListening() const;
		HttpRequest &getRequest();
		std::optional<HttpResponse> getResponse() const;
		const ServerConfig &getConfig() const;
		void		buildRequest();
		void		buildResponse();
		void		clearClear();
		void		setCgiFD(int fd);
		int 		getCgiFD();
	private:
		void		buildErrorResponse();

		static const	std::string DEFAULT_BODY;
};

#endif
