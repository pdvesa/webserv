#include "HttpRequest.hpp"
#include <iostream>
#include <unistd.h>
#include "Client.hpp"

//HttpRequest::HttpRequest(const Client &client) : serv(client.getConfig()), requestStatus(200)
//{
//	readSocket(client.getClientFD());
//}


HttpRequest::HttpRequest(const HttpRequest& other) : serv(other.serv)
{
		requestMethod = other.requestMethod;
		requestTarget = other.requestTarget;
		requestVersion = other.requestVersion;
		requestHeader = other.requestHeader;
		rawBody = other.rawBody;
		fullRequest = other.fullRequest;
		requestPath = other.requestPath;
//		requestBody = other.requestBody;
		requestStatus = other.requestStatus;
}
HttpRequest& HttpRequest::operator=(const HttpRequest& other)
{
	if (this != &other)
	{
		requestMethod = other.requestMethod;
		requestTarget = other.requestTarget;
		requestVersion = other.requestVersion;
		requestHeader = other.requestHeader;
		rawBody = other.rawBody;
		fullRequest = other.fullRequest;
		requestPath = other.requestPath;
//		requestBody = other.requestBody;
		requestStatus = other.requestStatus;
	}
	return (*this);
}


HttpRequest::HttpRequest(const ServerConfig &cfg, int fd) : serv(cfg), requestStatus(200)
{
	readSocket(fd);
}

void HttpRequest::readSocket(int socket)
{
	int							rv;
	std::vector<unsigned char>	buffer(BUF_SIZE);
	std::cout << "buffer size: " << BUF_SIZE << std::endl;
	while ((rv = read(socket, buffer.data(), BUF_SIZE)) > 0) {
		buffer.resize(rv);
		fullRequest.insert(fullRequest.end(), buffer.begin(), buffer.end()); // might read too much into buffer
		if (rv < BUF_SIZE) 
			break;
	}
	std::cout << "Size of fullreq: " << fullRequest.size() << std::endl;
	write(1, fullRequest.data(), fullRequest.size());
	if (rv == -1)
		throw std::runtime_error("failed to read from socket");
	std::string reqstr(fullRequest.begin(), fullRequest.end());
	fillRequest(reqstr);
}

HttpRequest::~HttpRequest(){}

void	HttpRequest::fillRequest(std::string req)
{
	std::string	req_line = req.substr(0, req.find("\r\n"));
	std::string mtv[3];
	int			pos;
	for (int i = 0; i < 3; i++)
	{
		pos = req_line.find(' ');
		mtv[i] = req_line.substr(0, pos);
		req_line.erase(0, pos + 1);
	}
	requestMethod = mtv[0];
	if (mtv[0] != "GET" && mtv[0] != "POST" && mtv[0] != "DELETE")
	{
		requestStatus = 405; // METHOD NOT ALLOWED
		return ;
	}
	requestTarget = mtv[1]; // check if target is valid
	// some sort of :
	// if (requestTarget != validTarget)
	// {
	//	requestStatus = 404;
	//	return ;
	//	}
	requestVersion = mtv[2]; // check if version is correct one
	if (requestVersion != "HTTP/1.1")
	{
		requestStatus = 505; // HTTP VERSION NOT SUPPORTED
		return ;
	}
	req.erase(0, req.find("\r\n") + 2); // need to make scalable just testing
	fillHeaders(req);
//	fillRawBody(req);
//	printElements(); // debug atm
}

void	HttpRequest::fillHeaders(std::string &req)
{
//	std::string::iterator i = req.begin();
	while (req.substr(0, 2) != "\r\n")
	{
		std::string key = req.substr(0, req.find(':'));
		req.erase(0, req.find(':') + 1);
		std::string value = req.substr(0, req.find("\r\n"));
//		if (value[0] != ' ')
//			throw std::runtime_error("bad header formatting, needs to have space after :");
		req.erase(0, req.find("\r\n") + 2);
		requestHeader.insert({key,value});
	}
	try{
		 requestHeader.at("Host"); // checking that Host field exists in header
	}
	catch (std::exception &e) // just doing this until figure out how to handle errors
	{
		requestStatus = 404; // change to correct error status
		requestPath = serv.getErrorsPages().at(requestStatus);
		requestPath = "." + requestPath;
		return ;
	}
	buildPath();
	fillRawBody(req);
//	validateRequest();
	std::cout << "Path in fillHeaders: " << requestPath << std::endl;
}
void	HttpRequest::printElements() const
{
	std::cout << "Request Method:" << requestMethod << std::endl;
	std::cout << "Request Target:" << requestTarget << std::endl;
	std::cout << "Request Version:" << requestVersion << std::endl;
	for (const auto &i : requestHeader)
		std::cout << "Key:" << i.first << " Value:" << i.second << std::endl;
//	for (const auto &i : rawBody)
//		std::cout << "Vector element: " << i << std::endl;
}

void	HttpRequest::fillRawBody(std::string &req)
{
	std::cout << "Body filled with content :";
	rawBody.reserve(req.size());
	for (const auto &i : req)
	{
		rawBody.push_back(i);
		std::cout << i;
	}
	std::cout << std::endl;
}

/*
void	HttpRequest::populateChunks(std::vector<unsigned char>& vec)
{
	const char *eof = "\r\n\r\n";
	auto it = std::search(vec.begin(), vec.end(), eof, eof + strlen(eof));
	std::string sizestr(vec.begin(), it);
	long reqSize = strtol(sizestr.c_str(), NULL, 16);
	std::string body_content = "placeholder";
	requestBody.emplace_back(reqSize,body_content);
}
*/
static void prepPath(std::string& requestPath, std::string index, bool add_index)
{
	requestPath.insert(0, ".");
	if (add_index)
		requestPath += index;
}


static std::vector<std::string> splitURI(std::string URI)
{
	std::vector<std::string> result;
	size_t start = 0;
	size_t end = 0;
	while ((end = URI.find('/', start)) != std::string::npos)
	{
		result.push_back(URI.substr(start, end - start + 1));
		start = end + 1;
	}
	if (start < URI.length())
        result.push_back(URI.substr(start));
	return result;
}


void HttpRequest::buildPath()
{
//	if (requestTarget == "/")
//		requestPath = serv.getRoutes().at(requestTarget).getRootDir();
	std::string latestroot;
	std::vector<std::string> paths = splitURI(requestTarget);
	for (const auto &i : paths)
		std::cout << "Path: " << i << std::endl;
	std::string index;
	bool add_index = 1;
	std::map<std::string ,RouteConfig>	routes = serv.getRoutes();
	RouteConfig::t_redirection redir;
	
//	std::cout << "Target URI: " << requestTarget << std::endl;
	if (routes.count(requestTarget))
	{
		std::cout << "request with location match found!\n";
		std::cout << requestTarget << std::endl;
		requestPath = routes.at(requestTarget).getRootDir();
		prepPath(requestPath, routes.at(requestTarget).getIndex(), add_index);
		redir = routes.at(requestTarget).getRedirection(); // will return t_redirect struct;
		if (!redir.path.empty())
		{
			requestPath = redir.path;
			requestStatus = redir.code;
		}
		// TODO: redirections and checking that method is allowed in that route
		// also add redirection to config file and test to see what syntax is in serverConfig!
		return ;
	}
//	std::vector<std::string> paths = CppSplit::cppSplit(requestTarget, '/');
	for (auto &i : paths)
	{
		try {
			if (routes.count(i))
			{
				requestPath = routes.at(i).getRootDir();
				index = routes.at(i).getIndex();
				latestroot = requestPath;
			}
			else 
			{
				i.insert(0, "/");
				requestPath = routes.at(i).getRootDir();
				index = routes.at(i).getIndex();
				latestroot = requestPath;
			}
		}
		catch (std::exception &e)
		{
			std::cerr << "Failed to find location : " << i << " in routesMap, appending to previous route\n";
			requestPath += i;
			add_index = 0;
		}
	}
//	std::cout << "final path in buildPath: " << requestPath << std::endl;
	requestPath.insert(0, ".");
	if (add_index)
		requestPath += index;
	else
		requestPath = requestPath.substr(0, requestPath.size());
//	std::cout << "final path in buildPath with resource: " << requestPath << std::endl;
	std::cerr << "Before checking if path exists: " << requestPath << std::endl;
	if (!std::filesystem::exists(requestPath))
	{
		requestStatus = 404;
		requestPath = serv.getErrorsPages().at(requestStatus);
		requestPath = "." + requestPath;
		std::cerr << "404 in buildPath: \n";
		return ;
	}
		// need to append index of latest location;

}
/*
void	HttpRequest::validateRequest()
{
	std::string path;
	std::string index;

//	std::cout << "Size of the map: " << serv.getRoutes().size() << std::endl;
//	for (const auto &pair : serv.getRoutes())
//	{
//		std::cout << "Key: "<< pair.first << std::endl;
//	}

	std::string loc = requestTarget.substr(0, requestTarget.find_last_of('/') + 1);
//	std::cout << "request target " << requestTarget << std::endl;
//	std::cout << "loc target " << loc << std::endl;
//	std::string allowedMethods[3] {"GET", "POST", "DELETE"};
	try{
		path = serv.getRoutes().at(loc).getRootDir(); // hould only access location even if there are things after
	
		if (requestTarget == loc)
			index = serv.getRoutes().at(requestTarget).getIndex();
		else
			index = requestTarget.substr(requestTarget.find_last_of('/') + 1);
		path += index;
		path = "." + path;
		requestPath = path;
//		std::cout << "path before checking if exists: " << requestPath << std::endl;
		if (!std::filesystem::exists(requestPath))
		{
			requestStatus = 404;
			requestPath = serv.getErrorsPages().at(requestStatus);
			requestPath = "." + requestPath;
		}
//		std::cout << "Path in request validation: " << requestPath << std::endl;
	}
	catch (std::exception &e)
	{
		requestStatus = 404;
		requestPath = serv.getErrorsPages().at(requestStatus);
		requestPath = "." + requestPath;
//		std::cout << "Error path is now: " << requestPath << std::endl;
		return ;
	}
}*/
