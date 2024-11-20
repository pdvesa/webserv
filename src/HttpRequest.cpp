#include "HttpRequest.hpp"
#include <iostream>
#include <unistd.h>
#include "Client.hpp"

//HttpRequest::HttpRequest(const Client &client) : serv(client.getConfig()), requestStatus(200)
//{
//	readSocket(client.getClientFD());
//}


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
		requestStatus = 400;
		return ;
	}
	validateRequest();
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
	rawBody.reserve(req.size());
	for (const auto &i : req)
		rawBody.push_back(i);
}


void	HttpRequest::populateChunks(std::vector<unsigned char>& vec)
{
	const char *eof = "\r\n\r\n";
	// get size_str
	// strtol hexadecimal into a long, using strtol(str, NULL, 16);
	// skip \r\n\r\n after size, then read size amount of bytes as body
	// pass size and body into BodyChunk;
	auto it = std::search(vec.begin(), vec.end(), eof, eof + strlen(eof));
	std::string sizestr(vec.begin(), it);
	long reqSize = strtol(sizestr.c_str(), NULL, 16);
	// find new string until next eof str
	std::string body_content = "placeholder";
	requestBody.emplace_back(reqSize,body_content);
	// need to find good way to loop thru vec
}


void	HttpRequest::validateRequest()
{
	std::string path;
	std::string index;
//	std::string allowedMethods[3] {"GET", "POST", "DELETE"};
	try{
		path = serv.getRoutes().at(requestTarget).getRootDir();
		index = serv.getRoutes().at(requestTarget).getIndex();
		path += requestTarget.substr(1);
		path += index;
		path = "." + path;
		requestPath = path;
		if (!std::filesystem::exists(requestPath))
		{
			requestStatus = 404;
			requestPath = serv.getErrorsPages().at(requestStatus);
		}
		std::cout << "Path in request validation: " << requestPath << std::endl;
	}
	catch (std::exception &e)
	{
		requestStatus = 404;
		requestPath = serv.getErrorsPages().at(requestStatus);
		requestPath = "." + requestPath;
		std::cout << "Error path is now: " << requestPath << std::endl;
		return ;
	}
}
