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
}

std::optional<HttpResponse> Client::getResponse() const {
	return response;
}

void	Client::buildRequest() {
	if (request.has_value()) {
		std::cout << "GOT CALLED" << std::endl;
		request->appendR(getClientFD());
		std::cout << "OK " << request->getMethod() << std::endl;
	}
	else
		request.emplace(HttpRequest(getConfig(), getClientFD()));
}

void	Client::buildResponse() {
	try {
		if (request->getStatus() >= 200 && request->getStatus() < 300) {
			std::string	path = request->getPath();

			if (request->getCGIStatus()) {
			response.emplace(HttpResponse(*request, HandleRequest::handleCGI(*request), "text/html"));
		  	}
      		else if (request->getMethod() == "GET")
			{
				std::string	contentType;
				response.emplace(HttpResponse(*request, HandleRequest::handleGet(request->getTarget(),path,
					true, contentType), contentType));
			}
			else if (request->getMethod() == "POST")
			{
				response.emplace(HttpResponse(*request, HandleRequest::handlePost(path,
					request->getHeaders().at("Content-Type"), request->getBody()), "text/html"));
			}
			else if (request->getMethod() == "DELETE")
			{
				response.emplace(HttpResponse(*request, HandleRequest::handleDelete(path), "text/html"));
			}
		} else if (request->getStatus() >= 300 && request->getStatus() < 400) {
			response.emplace((HttpResponse(*request, request->getTarget())));
		} else {
			buildErrorResponse();
		}
	} catch (std::exception &e) {
		u_int	statusCode;
		try {
			statusCode = std::stoi(e.what());
		} catch (...) {
			statusCode = 500;
		}
		request->setStatus(static_cast<int>(statusCode));
		buildErrorResponse();
	}
}

void	Client::clearClear() {
	request.reset();
	response.reset();
}

void Client::buildErrorResponse() {

	std::string	body = DEFAULT_BODY;

	if (request->getServerConfig().getErrorsPages().contains(request->getStatus())) {
		try {
			if (std::ifstream errorPage(request->getServerConfig().getErrorsPages()[request->getStatus()]); errorPage.is_open()) {
				std::stringstream	buffer;
				buffer << errorPage.rdbuf();
				body = buffer.str();
			}
		} catch (std::exception&) { }
	}
	response.emplace(HttpResponse(*request, body, "text/html"));
}

const std::string Client::DEFAULT_BODY = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\""
			" content=\"width=device-width, initial-scale=1.0\"><title>Saku's Error</title><style>body "
			"{font-family: Arial, sans-serif; display: flex; justify-content: center; align-items: center;"
			"height: 100vh; margin: 0; background-color: #f0f8ff;} h1 {color: #333;}</style></head><body>"
			"<h1>{{{ERROR}}}</h1></body></html>";