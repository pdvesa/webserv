#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include "BodyChunk.hpp"
#include <cstring>
#include <sys/socket.h>
#include "ServerConfig.hpp"
#include <filesystem>
//#include "Client.hpp"
class ServerConfig;
class Client;
typedef enum e_cgi
{
	NO_CGI = 0,
	PY = 1,
	CGI = 2
} t_cgi;
class HttpRequest {
	private:
		std::string							requestMethod;
		std::string							requestTarget;
		std::string							requestVersion;
		std::map<std::string, std::string>	requestHeader;
		std::vector<unsigned char>			rawBody; // maybe not needed anymore
		std::vector<unsigned char>			fullRequest;
		const ServerConfig&					serv;
		std::string							requestPath;
		std::string							requestedResource;
//		std::vector<BodyChunk>				requestBody; // more manageable body chunks
		int									cgiStatus;
		int									requestStatus;
		bool								hasListing;
	public:
		HttpRequest& operator=(const HttpRequest& other);
		HttpRequest(const ServerConfig& cfg, int fd);
		HttpRequest(const HttpRequest& other);
		~HttpRequest();
		void	readSocket(int socket);
		void	fillRequest(std::string req);
		void	fillRequest_vec(std::vector<unsigned char>& req);
		void	fillHeaders(std::string &req);
		void	printElements() const;
		void	fillRawBody(std::string &req);
		void	populateChunks(std::vector<unsigned char> &vec);
		void	validateRequest();
		void	fulfillRequest();
		void	buildPath();
		RouteConfig	findRoute();
		bool checkRedirs(const RouteConfig& rt);
		void	serveError(int status);
		void	validateRoute(const RouteConfig&);
		void	updateCGI();
		std::vector<unsigned char>& getBody(){return rawBody;}
		const std::string& getMethod() {return requestMethod;}// should these return const & to string or object????
		const std::string& getTarget() {return requestTarget;}
		const std::string& getVersion(){return requestVersion;}
		const std::string& getPath(){return requestPath;}
		int					getStatus(){return requestStatus;}
		bool				getListing(){return hasListing;}
		const ServerConfig&  getServerConfig(){return serv;}
		std::map<std::string, std::string>& getHeaders(){return requestHeader;}

		void				setStatus(const int status) {this->requestStatus = status;}
};

#endif
