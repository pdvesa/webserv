#ifndef CLIENT_HPP
# define CLIENT_HPP

#ifndef BUF_SIZE
# define BUF_SIZE 14
#endif

#include <iostream>
#include <arpa/inet.h>
#include <ServerConfig.hpp>
#include "HttpResponse.hpp"
#include <optional>
#include "HandleRequest.hpp"

class HttpRequest;
class HttpResponse;
class Client {
	private:
		int				clientFD;
		int				listeningSocket;
		ServerConfig	config;
		std::optional<HttpRequest>		request;
		std::optional<HttpResponse>		response;
		std::vector<unsigned char>		rawRequest;
		int 			cgiFD;
		bool			requestRdy;
		bool			responseRdy;
	public:
		Client(int connection, int listen, ServerConfig conf);
		~Client();
		int getClientFD();
		int getListening() const;
		std::optional<HttpRequest> getRequest() const;
		std::optional<HttpResponse> getResponse() const;
		const ServerConfig &getConfig() const;
		void		buildRequest();
		void		buildResponse();
		void		clearClear();
		void		setCgiFD(int fd);
		int 		getCgiFD();
		bool		getRequestStatus();
		bool		getResponseStatus();
		void		setRequestStatus(bool stat);
		void		setResponseStatus(bool stat);
		std::vector<unsigned char> &getRawRequest();
	private:
		void		buildErrorResponse();

		static const	std::string DEFAULT_BODY;
};

#endif
