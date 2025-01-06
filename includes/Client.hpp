#ifndef CLIENT_HPP
# define CLIENT_HPP

#ifndef BUF_SIZE
# define BUF_SIZE 10280
#endif

#include <memory>
#include <iostream>
#include <arpa/inet.h>
#include <ServerConfig.hpp>
#include <vector>
#include <chrono>
#include <HttpRequest.hpp>
#include <HttpResponse.hpp>


class HttpRequest;
class HttpResponse;

enum cgi_state {
	NO_CGI,
	CGI_WAIT,
	CGI_RDY
};

class Client {
	private:
		int				clientFD;
		int				listeningSocket;
		std::shared_ptr<ServerConfig>	config;
		HttpRequest		request;
		HttpResponse	response;
		int 			cgiFD;
		int				cgiState;
		pid_t			pid;
		std::vector<unsigned char> cgiResp;
		std::chrono::steady_clock::time_point timestamp;
	public:
		Client(int connection, int listen, std::shared_ptr<ServerConfig> conf);
		~Client();
		int getClientFD();
		int getListening() const;
		HttpRequest &getRequest();
		HttpResponse &getResponse();
		const std::shared_ptr<ServerConfig> getConfig() const;
		void		setCgiFD(int fd);
		int 		getCgiFD();
		void		setCgiStatus(int state);
		int			getCgiStatus();
		pid_t		getPid();
		void		setPid(pid_t p);
		std::vector<unsigned char> &getCgiResponse();
		std::chrono::steady_clock::time_point getTimestamp();
};

#endif
