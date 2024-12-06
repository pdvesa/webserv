#include "HttpRequest.hpp"
#include <iostream>
#include <unistd.h>
#include "Client.hpp"

HttpRequest::HttpRequest(const HttpRequest& other) : serv(other.serv)
{
		requestMethod = other.requestMethod;
		requestTarget = other.requestTarget;
		requestVersion = other.requestVersion;
		requestHeader = other.requestHeader;
		rawBody = other.rawBody;
		fullRequest = other.fullRequest;
		requestPath = other.requestPath;
		requestStatus = other.requestStatus;
		requestedResource = other.requestedResource;
		cgiStatus = other.cgiStatus;
		hasListing = other.hasListing;
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
		requestStatus = other.requestStatus;
		requestedResource = other.requestedResource;
		cgiStatus = other.cgiStatus;
		hasListing = other.hasListing;
	}
	return (*this);
}


HttpRequest::HttpRequest(const ServerConfig &cfg, int fd) : serv(cfg), requestedResource(""), cgiStatus(0), requestStatus(200)
{
	readSocket(fd);
}

void HttpRequest::readSocket(int socket)
{
	int							rv;
	std::vector<unsigned char>	buffer(BUF_SIZE);
//	std::cout << "buffer size: " << BUF_SIZE << std::endl;
	while ((rv = read(socket, buffer.data(), BUF_SIZE)) > 0) {
		buffer.resize(rv);
		fullRequest.insert(fullRequest.end(), buffer.begin(), buffer.end()); // might read too much into buffer
		if (rv < BUF_SIZE) 
			break;
	}
//	std::cout << "Size of fullreq: " << fullRequest.size() << std::endl;
//	write(1, fullRequest.data(), fullRequest.size());
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
		requestStatus = 405;
		serveError(405);
		return ;
	}
	requestTarget = mtv[1];
	requestVersion = mtv[2]; // check if version is correct one
	if (requestVersion != "HTTP/1.1")
	{
		std::cout << "version mismatch \n" << std::endl;
		requestStatus = 505; // HTTP VERSION NOT SUPPORTED
		return ;
	}
	req.erase(0, req.find("\r\n") + 2); // need to make scalable just testing
	fillHeaders(req);
	fillRawBody(req);
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
	std::cout << "Path in fillHeaders: " << requestPath << std::endl;
//	for(auto & key: requestHeader)
//	{
//		std::cout << "Key: " << key.first << " Value: " << key.second << std::endl;
//	}
}
void	HttpRequest::printElements() const
{
	std::cout << "Request Method:" << requestMethod << std::endl;
	std::cout << "Request Target:" << requestTarget << std::endl;
	std::cout << "Request Version:" << requestVersion << std::endl;
	for (const auto &i : requestHeader)
		std::cout << "Key:" << i.first << " Value:" << i.second << std::endl;
}

void	HttpRequest::fillRawBody(std::string &req)
{
	rawBody.reserve(req.size());
	for (const auto &i : req)
	{
		rawBody.push_back(i);
//		std::cout << i;
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
RouteConfig HttpRequest::findRoute() {
    size_t latestSlash = requestTarget.rfind('/');
	std::string routeMatch = requestTarget;
    std::map<std::string, RouteConfig> routes = serv.getRoutes();
    if (routes.find(requestTarget) != routes.end()) {
        return routes.at(requestTarget);
    }
    while (latestSlash != std::string::npos) {
        routeMatch = requestTarget.substr(0, latestSlash + 1);
        if (routes.find(routeMatch) != routes.end()) {
			requestedResource = requestTarget.substr(latestSlash + 1);
			std::cout << "returning with route at: " << routeMatch << " and requested resource: " << requestedResource << std::endl;
            return routes.at(routeMatch);
        }
        routeMatch = routeMatch.substr(0, latestSlash);
        latestSlash = routeMatch.rfind('/');
    }
    if (routes.find("/") != routes.end()) {
        return routes.at("/");
    }

    throw std::runtime_error("Failed to find a route for: " + requestTarget);
}

 bool HttpRequest::checkRedirs(const RouteConfig& rt)
{
	if (!rt.getRedirection().path.empty())
	{
		RouteConfig::t_redirection redir = rt.getRedirection();
		requestStatus = redir.code;
		requestTarget = redir.path;
		if (!requestedResource.empty())
			requestTarget.append(requestedResource);
		return true;
	}
	return false;
}

void HttpRequest::validateRoute(const RouteConfig& rt)
{
	hasListing = rt.getListing();
	if (requestMethod == "POST")
	{
		if (!rt.getPOST())
			throw std::runtime_error("method not supported");
	}
	else if (requestMethod == "DELETE")
	{
		if (!rt.getDELETE())
			throw std::runtime_error("method not supported");
	}
	else if (requestMethod == "GET")
	{
		if (!rt.getGET())
			throw std::runtime_error("method not supported");
	}
}
void HttpRequest::buildPath()
{
	try
	{
		RouteConfig route = findRoute();
		if(checkRedirs(route))
			return ;
		validateRoute(route);
		requestPath = route.getRootDir();
		if (requestedResource.empty() && requestMethod == "GET")
			requestPath.append(route.getIndex());
		else
			requestPath.append(requestedResource);
		requestPath.insert(0, ".");
		updateCGI();
		if (requestMethod == "POST" && !cgiStatus)
		{
			if (!std::filesystem::exists(requestPath))
			{
				std::cout << "serving error here with path: " << requestPath << std::endl;
				serveError(404);
				return ;
			}
			requestPath.append(route.getUploadDir() + "/");
			return ;
		}
		if (!std::filesystem::exists(requestPath))
			serveError(404);
	}
	catch (std::exception &e)
	{
		std::cout << e.what();
		std::cout << "404 in buildPath\n";
		requestStatus = 404;
		requestPath = serv.getErrorsPages().at(404);
	}
}
void	HttpRequest::serveError(int status)
{
	requestStatus = status;
	requestPath = serv.getErrorsPages().at(status);
	requestPath.insert(0, ".");
}

void	HttpRequest::updateCGI()
{
	std::cout << "in updateCGI: " << "path: " << requestPath << std::endl;
	if (requestPath.find("/cgi-bin/") != std::string::npos)
	{
		if (requestedResource.substr(requestedResource.length() - 3) == ".py")
		{
			cgiStatus = PY;
			return ;
		}
		if (requestedResource.substr(requestedResource.length() - 4) == ".cgi")
		{
			cgiStatus = CGI;
			return ;
		}
	}
	cgiStatus = NO_CGI;
	return ;
}
