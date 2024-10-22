#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include "BodyChunk.hpp"
#include <cstring>
#include <sys/socket.h>

class HttpRequest {
	private:
		// need to add statuscode somewhere, could throw and just catch & print it;
		std::string							requestMethod;
		std::string							requestTarget;
		std::string							requestVersion;
		std::map<std::string, std::string>	requestHeader;
		std::vector<unsigned char>			rawBody; // maybe not needed anymore
		std::vector<unsigned char>			fullRequest;
		bool								hasBody;
		std::vector<BodyChunk>				requestBody; // more manageable body chunks
	public:
		HttpRequest(std::string req);
		HttpRequest(int socket);
		~HttpRequest();
		void	readSocket(int socket);
		void	fillRequest(std::string req);
		void	fillHeaders(std::string &req);
		void	printElements() const;
		void	fillRawBody(std::string &req);
		void	populateChunks(std::vector<unsigned char> &vec);
		std::string getMethod() {return requestMethod;}// should these return const & to string or object????
		std::string getTarget() {return requestTarget;}
		std::string getVersion(){return requestVersion;}
};

#endif
