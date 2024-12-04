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

std::optional<HttpRequest> Client::getRequest() const {
	return request;
//	throw std::runtime_error("request not formed yet");
}

std::optional<HttpResponse> Client::getResponse() const {
	return response;
//	throw std::runtime_error("response not formed yet");
}

void	Client::buildRequest() {
	request.emplace(HttpRequest(getConfig(), getClientFD()));
}

void	Client::buildResponse() {
	try {
		if (request->getMethod() == "GET")
			response.emplace(HttpResponse(*request, HandleRequest::handleGet(request->getTarget(), request->getPath(), true)));
		else if (request->getMethod() == "POST")
			response.emplace(HttpResponse(*request, HandleRequest::handlePost(request->getPath(), request->getHeaders().at("Content-Type"), request->getBody())));
		else if (request->getMethod() == "DELETE")
			response.emplace(HttpResponse(*request, HandleRequest::handleDelete(request->getPath())));
	} catch (std::exception &e) {
		int	statusCode;
		try {
			statusCode = std::stoi(e.what());
		} catch (...) {
			statusCode = 500;
		}
		request->setStatus(statusCode);
		response.emplace(HttpResponse(*request, ""));
	}
}

void	Client::clearClear() {
	request.reset();
	response.reset();
}
