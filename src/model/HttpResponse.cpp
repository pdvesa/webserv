#include "HttpResponse.hpp"

HttpResponse::HttpResponse(HttpRequest request, const std::string& responseBody) {
	this->responseStatusLine = createResponseStatusLine(request.getStatus());
	this->responseBody = responseBody;
	this->contentLengthLine = createContentLengthLine();
	this->connectionLine = createConnectionLine();
}

HttpResponse::HttpResponse(const HttpResponse& other) {
	*this = other;
}

HttpResponse::~HttpResponse() { }

HttpResponse& HttpResponse::operator=(const HttpResponse& other) {
	if (this != &other) {
		responseStatusLine = other.responseStatusLine;
		responseBody = other.responseBody;
		contentLengthLine = other.contentLengthLine;
		connectionLine = other.connectionLine;
	}
	return (*this);
}

std::string HttpResponse::toString() const {
	return (responseStatusLine + "\n"
		+ contentLengthLine + "\n"
		+ connectionLine + "\n\n" +
		responseBody);
}

std::string	HttpResponse::createResponseStatusLine(const int code) const {
	const std::string message = httpErrors.at(code);
	return (HTTP_VERSION + ' ' + std::to_string(code) + ' ' + message);
}

std::string HttpResponse::createContentLengthLine() const {
	return (CONTENT_LENGTH + ": " + std::to_string(responseBody.size()));
}

std::string HttpResponse::createConnectionLine() const {
	return (CONNECTION + ": " + "keep-alive"); //TODO No idea what to put
}
