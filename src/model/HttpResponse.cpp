#include "HttpResponse.hpp"

HttpResponse::HttpResponse(HttpRequest request, const std::string& responseBody, const std::string& contentType) {
	this->responseStatusLine = createResponseStatusLine(request.getStatus());
	this->responseBody = responseBody;
	if (request.getStatus() >= 400)
		errorBuilder(this->responseBody, request.getStatus());
	this->contentLengthLine = createContentLengthLine();
	this->connectionLine = createConnectionLine();
	this->contentTypeLine = createContentTypeLine(contentType);
	this->locationLine = "";
}

HttpResponse::HttpResponse(HttpRequest request, const std::string& redirectionTarget) {
	this->responseStatusLine = createResponseStatusLine(request.getStatus());
	this->locationLine = createLocationLine(redirectionTarget);
	this->responseBody = "";
	this->contentLengthLine = createContentLengthLine();
	this->connectionLine = createConnectionLine();
	this->contentTypeLine = createContentTypeLine("text/html");
}

HttpResponse::HttpResponse(const HttpResponse& other) {
	*this = other;
}

HttpResponse::~HttpResponse() { }

HttpResponse& HttpResponse::operator=(const HttpResponse& other) {
	if (this != &other) {
		responseStatusLine = other.responseStatusLine;
		locationLine = other.locationLine;
		responseBody = other.responseBody;
		contentLengthLine = other.contentLengthLine;
		connectionLine = other.connectionLine;
		contentTypeLine = other.contentTypeLine;
	}
	return (*this);
}

std::string HttpResponse::toString() const {
	return (responseStatusLine
		+ locationLine
		+ contentTypeLine
		+ contentLengthLine
		+ connectionLine + "\n" +
		responseBody);
}

std::string	HttpResponse::createResponseStatusLine(const int code) const {
	const std::string message = httpErrors.at(code);
	return (HTTP_VERSION + ' ' + std::to_string(code) + ' ' + message + "\n");
}

std::string HttpResponse::createContentLengthLine() const {
	return (CONTENT_LENGTH + ": " + std::to_string(responseBody.size()) + "\n");
}

std::string HttpResponse::createConnectionLine() const {
	return (CONNECTION + ": " + "keep-alive" + "\n");
}

std::string HttpResponse::createContentTypeLine(const std::string& contentType) const {
	return (CONTENT_TYPE + ": " + contentType + "\n");
}

std::string HttpResponse::createLocationLine(const std::string& redirectionTarget) const {
	return (LOCATION + ": " + redirectionTarget + "\n");
}

void HttpResponse::errorBuilder(std::string &response, const int status) {
	std::string errorStr = std::to_string(status) + " " + httpErrors[status];
	int	index = response.find("{{{ERROR}}}");
	if (index != (int)std::string::npos)
		response.replace(index, 11, errorStr);
	else
		std::cout << "There is literally no reason I will ever print this, fix your shit" << std::endl;
}
