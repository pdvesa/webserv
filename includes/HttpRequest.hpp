#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

#include <string>
#include <map>
#include <vector>

class HttpRequest {
	private:
		std::string							requestMethod;
		std::string							requestTarget;
		std::string							requestversion;
		std::map<std::string, std::string>	requestHeader;
		std::vector<unsigned char>			requestBody; //dont know if we do multiparts idk
};

#endif
