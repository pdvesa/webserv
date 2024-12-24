//
// Created by jules on 20/12/2024.
//

#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

#include <string>
#include <vector>

#include <HttpRequest.hpp>
#include <HandleRequest.hpp>

class HttpResponse {
	private:
		int					statusCode;
		std::string			location;
		std::string			contentType;
		int					contentLength;
		std::string			connection;
		std::vector<u_char>	responseBody;

	public:
		HttpResponse() = delete;
		explicit HttpResponse(const HttpRequest & request);
		HttpResponse(const HttpResponse & other) = default;
		~HttpResponse() = default;

		HttpResponse	&operator=(const HttpResponse & other) = default;
		bool			operator==(const HttpResponse&) const = default;
		bool			operator!=(const HttpResponse&) const = default;

		[[nodiscard]] std::vector<u_char>	asResponseBuffer() const;

	private:
		[[nodiscard]] inline std::string	createResponseStatusLine() const;
		[[nodiscard]] inline std::string	createLocationLine() const;
		[[nodiscard]] inline std::string	createContentTypeLine() const;
		[[nodiscard]] inline std::string	createContentLengthLine() const;
		[[nodiscard]] inline std::string	createConnectionLine() const;
};

static const std::string	HTTP_VERSION = "HTTP/1.1";
static const std::string	CONTENT_LENGTH = "Content-Length";
static const std::string	CONNECTION = "Connection";
static const std::string	CONTENT_TYPE = "Content-Type";
static const std::string	LOCATION = "Location";
static const std::string	HTTP_NEWLINE = "\r\n";

#endif //HTTPRESPONSE_HPP
