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
		std::vector<BodyChunk>				requestBody; // more manageable body chunks
		int									requestStatus;
	public:
//		HttpRequest();
//		HttpRequest(std::string req);
//		HttpRequest(const Client &client);
		HttpRequest& operator=(const HttpRequest& other);
		HttpRequest(const ServerConfig& cfg, int fd);
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
		const std::string& getMethod() {return requestMethod;}// should these return const & to string or object????
		const std::string& getTarget() {return requestTarget;}
		const std::string& getVersion(){return requestVersion;}
		const std::string& getPath(){return requestPath;}
		int					getStatus(){return requestStatus;}
};

#endif
