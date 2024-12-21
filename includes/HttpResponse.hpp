//
// Created by jules on 20/12/2024.
//

#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include <string>
#include <HttpRequest.hpp>
#include <HandleRequest.hpp>
#include <vector>
#include <map>

class HttpRequest;

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

static const std::map<int, std::string> httpErrors = {
	{200, "OK"},
	{300, "Redirect"},
	{301, "Moved Permanently"},
	{302, "Found"},
	{303, "See Other"},
	{400, "Bad Request"},
	{401, "Unauthorized"},
	{402, "Payment Required"},
	{403, "Forbidden"},
	{404, "Not Found"},
	{405, "Method Not Allowed"},
	{406, "Not Acceptable"},
	{407, "Proxy Authentication Required"},
	{408, "Request Timeout"},
	{409, "Conflict"},
	{410, "Gone"},
	{411, "Length Required"},
	{412, "Precondition Failed"},
	{413, "Payload Too Large"},
	{414, "URI Too Long"},
	{415, "Unsupported Media Type"},
	{416, "Range Not Satisfiable"},
	{417, "Expectation Failed"},
	{418, "I'm a teapot"},  // Fun Easter egg (RFC 2324)
	{421, "Misdirected Request"},
	{422, "Unprocessable Entity"},
	{423, "Locked"},
	{424, "Failed Dependency"},
	{425, "Too Early"},
	{426, "Upgrade Required"},
	{428, "Precondition Required"},
	{429, "Too Many Requests"},
	{431, "Request Header Fields Too Large"},
	{451, "Unavailable For Legal Reasons"},
	{500, "Internal Server Error"},
	{501, "Not Implemented"},
	{502, "Bad Gateway"},
	{503, "Service Unavailable"},
	{504, "Gateway Timeout"},
	{505, "HTTP Version Not Supported"},
	{507, "Insufficient Storage"},
	{508, "Loop Detected"},
	{510, "Not Extended"},
	{511, "Network Authentication Required"}
};

#endif //HTTPRESPONSE_HPP
