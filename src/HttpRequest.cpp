#include "HttpRequest.hpp"
#include <iostream>
HttpRequest::HttpRequest(std::string req)
{
	fillRequest(req);
}

HttpRequest::HttpRequest(int socket)
{
	readSocket(socket);
}

void HttpRequest::readSocket(int socket)
{
	int rv;
	 while ((rv = recv(socket,fullRequest.data(), 1024, 0)) > 0)
		 ;
	if (rv == -1)
		throw std::runtime_error("failed to read from socket");
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
	if (mtv[0] != "GET" && mtv[0] != "POST" && mtv[0] != "DELETE")
		throw std::runtime_error("Request method not supported"); // this needs to update status, not just throw exception
	requestMethod = mtv[0];
	requestTarget = mtv[1]; // check if target is valid
	requestVersion = mtv[2]; // check if version is correct one
	req.erase(0, req.find("\r\n") + 2); // need to make scalable just testing
	fillHeaders(req);
	fillRawBody(req);
	printElements(); // debug atm
	// need to add status at some point
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
}
void	HttpRequest::printElements() const
{
	std::cout << "Request Method:" << requestMethod << std::endl;
	std::cout << "Request Target:" << requestTarget << std::endl;
	std::cout << "Request Version:" << requestVersion << std::endl;
	for (const auto &i : requestHeader)
		std::cout << "Key: " << i.first << " Value: " << i.second << std::endl;
	for (const auto &i : rawBody)
		std::cout << "Vector element: " << i << std::endl;
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
