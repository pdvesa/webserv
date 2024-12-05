#include "HttpResponse.hpp"

HttpResponse::HttpResponse(HttpRequest request, const std::string& responseBody, const std::string& contentType) {
	this->responseStatusLine = createResponseStatusLine(request.getStatus());
	this->responseBody = responseBody;
	if (request.getStatus() >= 400)
		errorBuilder(this->responseBody, request.getStatus());
	this->contentLengthLine = createContentLengthLine();
	this->connectionLine = createConnectionLine();
	this->contentTypeLine = createContentTypeLine(contentType);
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
		contentTypeLine = other.contentTypeLine;
	}
	return (*this);
}

std::string HttpResponse::toString() const {
	return (responseStatusLine + "\n"
		+ contentTypeLine + "\n"
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

std::string HttpResponse::createContentTypeLine(const std::string& contentType) const {
	return (CONTENT_TYPE + ": " + contentType);
}

void HttpResponse::errorBuilder(std::string &response, const int status) {
	std::cout << response << std::endl;
	std::string errorStr = std::to_string(status) + " " + httpErrors[status]; //assuming everything here is correct as should :()()
	int	index = response.find("{{{ERROR}}}");
	if (index != (int)std::string::npos)
		response.replace(index, 11, errorStr);
	else
		std::cout << "There is literally no reason I will ever print this, fix your shit" << std::endl;
}
