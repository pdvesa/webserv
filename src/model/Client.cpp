#include <Client.hpp>

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
		buildErrorResponse(e);
	}
}

void	Client::clearClear() {
	request.reset();
	response.reset();
}

void Client::buildErrorResponse(std::exception &e) {
	u_int	statusCode;
	try {
		statusCode = std::stoi(e.what());
	} catch (...) {
		statusCode = 500;
	}
	std::string	body = DEFAULT_BODY;

	if (request->getServerConfig().getErrorsPages().contains(statusCode)) {
		try {
			if (std::ifstream errorPage(request->getServerConfig().getErrorsPages()[statusCode]); errorPage.is_open()) {
				std::stringstream	buffer;
				buffer << errorPage.rdbuf();
				body = buffer.str();
			}
		} catch (std::exception&) { }
	}
	request->setStatus(static_cast<int>(statusCode));
	response.emplace(HttpResponse(*request, body));
}

const std::string Client::DEFAULT_BODY = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\""
			" content=\"width=device-width, initial-scale=1.0\"><title>Saku's Error</title><style>body "
			"{font-family: Arial, sans-serif; display: flex; justify-content: center; align-items: center;"
			"height: 100vh; margin: 0; background-color: #f0f8ff;} h1 {color: #333;}</style></head><body>"
			"<h1>{{{ERROR}}}</h1></body></html>";