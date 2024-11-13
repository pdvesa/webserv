#pragma once
#include "HttpRequest.hpp"
#include <iostream>
#include <fstream>
#include <sstream>


enum e_code
{
	CONTINUE = 100,
	SWITCHING_PROTOCOLS = 101,
	PROCESSING = 102,
	OK = 200,
	CREATED = 201,
	ACCEPTED = 202,


};
class HttpResponse
{
	private:
		std::string	responseStatusLine;
		std::string	contentLengthLine;
		std::string	connectionLine;
		std::string	responseBody;

	public:
		HttpResponse(HttpRequest request);
		HttpResponse(HttpResponse& other);
		~HttpResponse();

		HttpResponse& operator=(const HttpResponse& other);

		std::string	toString() const;

	private:
		std::string	createResponseStatusLine(int code) const;
		std::string	getResponseBody(const std::string& target) const;
		std::string	createContentLengthLine() const;
		std::string	createConnectionLine() const;

		HttpResponse();

		const std::string	HTTP_VERSION = "HTTP/1.1";
		const std::string	CONTENT_LENGTH = "Content-Length";
		const std::string	CONNECTION = "Connection";
		std::map<int, std::string> httpErrors = {
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
};
