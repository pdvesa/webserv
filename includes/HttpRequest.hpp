#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

#include <string>
#include <map>
#include <vector>
#include <algorithm>

class HttpRequest {
	private:
		// need to add statuscode somewhere, could throw and just catch & print it;
		std::string							requestMethod;
		std::string							requestTarget;
		std::string							requestVersion;
		std::map<std::string, std::string>	requestHeader;
		std::vector<unsigned char>			requestBody; //chunked bodies
	public:
		HttpRequest();
		~HttpRequest();
		void	fillRequest(std::string req);
		void	fillHeaders(std::string &req);
		void	printElements() const;
		void	fillBody(std::string &req);
		std::string getMethod() {return requestMethod;}// should these return const & to string or object????
		std::string getTarget() {return requestTarget;}
		std::string getVersion(){return requestVersion;}
};

#endif
