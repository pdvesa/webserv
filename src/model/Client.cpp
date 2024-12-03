#include <Client.hpp>
#include "HandleRequest.hpp"

Client::Client(int connection, int listen, ServerConfig conf) : clientFD(connection), listeningSocket(listen), config(conf), request(), response() {
}

Client::~Client() {
}

int Client::getClientFD() {
	return (clientFD);
}

int Client::getListening() const {
	return (listeningSocket);
}

const ServerConfig &Client::getConfig() const {
	return (config);
}


HttpRequest Client::getRequest() const
{
	if (request.has_value())
		return *request;
	throw std::runtime_error("request not formed yet");
}

HttpResponse Client::getResponse() const
{
	if (response.has_value())
		return *response;
	throw std::runtime_error("response not formed yet");
}

void	Client::buildRequest()
{
	request.emplace(HttpRequest(getConfig(), getClientFD()));
}
void	Client::buildResponse()
{
	if (request.has_value()) {
		if (request->getMethod() == "GET")
			response.emplace(HttpResponse(*request, HandleRequest::handleGet(request->getTarget(), request->getPath(), true)));
		else if (request->getMethod() == "POST")
			response.emplace(HttpResponse(*request, HandleRequest::handlePost(request->getPath(), "", request->getBody())));
		else if (request->getMethod() == "DELETE")
			response.emplace(HttpResponse(*request, HandleRequest::handleDelete(request->getPath())));
	} else
		throw std::runtime_error("Request not ready");
}

void	Client::clearClear()
{
	request.reset();
	response.reset();
}
