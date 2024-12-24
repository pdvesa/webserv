//
// Created by jules on 20/12/2024.
//

#include <HttpResponse.hpp>

HttpResponse::HttpResponse(const HttpRequest& request)
{
	statusCode = HandleRequest::handleRequest(request, location, contentType, responseBody);
	contentLength = responseBody.size(); // NOLINT(*-narrowing-conversions)
	if (contentLength == 0 && request.getMethod() != GET)
		contentLength = -1;
	connection = "close";
}

std::vector<u_char> HttpResponse::asResponseBuffer() const
{
	std::ostringstream	responseHeaderBuf;

	responseHeaderBuf << createResponseStatusLine() << HTTP_NEWLINE;
	if (statusCode >= 300 && statusCode < 400)
		responseHeaderBuf << createLocationLine() << HTTP_NEWLINE;
	else
	{
		if (contentLength > 0)
			responseHeaderBuf << createContentTypeLine() << HTTP_NEWLINE;
		if (contentLength >= 0)
			responseHeaderBuf << createContentLengthLine() << HTTP_NEWLINE;
	}
	responseHeaderBuf << createConnectionLine() << HTTP_NEWLINE;
	responseHeaderBuf << HTTP_NEWLINE;

	std::vector<u_char> responseBuf(responseHeaderBuf.str().begin(), responseHeaderBuf.str().end());
	if (statusCode >= 300 && statusCode < 400 && contentLength > 0)
	{
		responseBuf.reserve(responseBuf.size() + responseBody.size());
		responseBuf.insert(responseBuf.end(), responseBody.begin(), responseBody.end());
	}
	return (responseBuf);
}

std::string HttpResponse::createResponseStatusLine() const
{
	return (HTTP_VERSION + ' ' + std::to_string(statusCode) + ' ' + httpCodes.at(statusCode));
}

std::string HttpResponse::createLocationLine() const
{
	return (LOCATION + ": " + location);
}

std::string HttpResponse::createContentTypeLine() const
{
	return (CONTENT_TYPE + ": " + contentType);
}

std::string HttpResponse::createContentLengthLine() const
{
	return (CONTENT_LENGTH + ": " + std::to_string(contentLength));
}

std::string HttpResponse::createConnectionLine() const
{
	return (CONNECTION + ": " + connection);
}
