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
		maxSize = other.maxSize;
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
		maxSize = other.maxSize;
	}
	return (*this);
}


HttpRequest::HttpRequest(const ServerConfig &cfg, int fd) : serv(cfg), requestedResource(""), cgiStatus(0), requestStatus(200), maxSize(cfg.getMaxClientBodySize())
{
	readSocket(fd);
}

void HttpRequest::readSocket(int socket)
{
	int							rv;
	std::vector<unsigned char>	buffer(BUF_SIZE);
	fullRequest.clear();

	while ((rv = read(socket, buffer.data(), BUF_SIZE)) > 0) {
		fullRequest.insert(fullRequest.end(), buffer.begin(), buffer.begin() + rv);
		if (rv < BUF_SIZE) 
			break;
	}
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
	requestVersion = mtv[2];
	if (requestVersion != "HTTP/1.1")
	{
		requestStatus = 505;
		return ;
	}
	req.erase(0, req.find("\r\n") + 2);
	fillHeaders(req);
	fillRawBody(req);
	if (rawBody.empty())
		requestHeader.insert({"Content-Type", " text/html"});
	if (requestHeader.count("Transfer-Encoding"))
	{
		if (requestHeader.at("Transfer-Encoding") == " chunked")
			rawBody = dechunkBody();
	}
	std::cout << "method in request: " << requestMethod << std::endl;
}

void	HttpRequest::fillHeaders(std::string &req)
{
	while (req.substr(0, 2) != "\r\n")
	{
		std::string key = req.substr(0, req.find(':'));
		req.erase(0, req.find(':') + 1);
		std::string value = req.substr(0, req.find("\r\n"));
		if (value[0] != ' ')
		{
			throw std::runtime_error("bad header formatting, needs to have space after :");
			requestStatus = 400;
		}
		req.erase(0, req.find("\r\n") + 2);
		requestHeader.insert({key,value});
	}
	if (!requestHeader.count("Host"))
	{
		requestStatus = 400; 
		requestPath = serv.getErrorsPages().at(404);
		requestPath = "." + requestPath;
		return ;
	}
	buildPath();
	req.erase(0, 2);
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
		rawBody.push_back(i);
	if (rawBody.size() > maxSize)
		requestStatus = 413;
}


 std::vector<unsigned char>	HttpRequest::dechunkBody()
{
	std::vector<unsigned char> dechunkedBody;

	auto iter = rawBody.begin();
	while (iter != rawBody.end())
	{
		try{
			auto chunkStart = iter;
			auto chunkEnd = std::find(iter, rawBody.end(), '\r');
			if (chunkEnd == rawBody.end() || chunkEnd + 1 == rawBody.end())
				throw std::runtime_error("bad chunked request\n");
			std::string chunkSize(chunkStart, chunkEnd);
			size_t chunkLen = std::stoi(chunkSize, nullptr, 16);
			if (chunkLen == 0)
				break ;
			chunkEnd += 2;
			std::copy(chunkEnd, chunkEnd + chunkLen, std::back_inserter(dechunkedBody));
			if (dechunkedBody.size() > maxSize)
			{
				requestStatus = 413;
				break ;
			}
			iter = chunkEnd + chunkLen + 2;
		}
		catch (std::exception &e)
		{
			requestStatus = 400;
			break ;
		}
	}
	return dechunkedBody;
}

RouteConfig HttpRequest::findRoute() {
    size_t latestSlash = requestTarget.rfind('/');
	std::string routeMatch = requestTarget;
    std::map<std::string, RouteConfig> routes = serv.getRoutes();
    if (routes.find(requestTarget) != routes.end()) {
        return routes.at(requestTarget);
    }
    while (latestSlash != std::string::npos) {
        routeMatch = requestTarget.substr(0, latestSlash + 1);
        if (routes.find(routeMatch) != routes.end())
		{
			requestedResource = requestTarget.substr(latestSlash + 1);
			if (!requestedResource.empty() && requestedResource[0] == '/')
				requestedResource.substr(1);
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
				serveError(404);
				return ;
			}
			requestPath.append(route.getUploadDir());
			return ;
		}
		if (!std::filesystem::exists(requestPath))
			serveError(404);
		else if (std::filesystem::is_directory(requestPath) && !requestPath.ends_with('/'))
			serveError(404);
	}
	catch (std::exception &e)
	{
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
	if (requestPath.find("/cgi-bin/") != std::string::npos)
	{
		if (requestedResource.substr(requestedResource.length() - 3) == ".py")
		{
			cgiStatus = PY;
			return ;
		}
		if (requestedResource.substr(requestedResource.length() - 4) == ".cgi")
		{
			cgiStatus = CGI_E;
			return ;
		}
	}
	cgiStatus = NO_CGI;
	return ;
}

std::string HttpRequest::getMapValue(std::string key) {
	auto found = requestHeader.find(key);
	if (found == requestHeader.end())
		return ("");
	else 
		return (found->second);
}
