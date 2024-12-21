//
// Created by jules on 20/12/2024.
//

#ifndef HANDLEREQUEST_HPP
#define HANDLEREQUEST_HPP

#include <HttpRequest.hpp>

class HandleRequest {
	public:
		static int	handleRequest(const HttpRequest& request, std::string& locationDest, std::string& contentTypeDest,
			std::vector<u_char>& bodyDest);

		HandleRequest() = delete;
		~HandleRequest() = delete;

	private:
		static RouteConfig&	parseTarget(const std::string& requestTarget, const std::string& requestHostname,
											const ServerConfig& server, std::string& remainingPathDest);

		static int	handleInvalidRequest(const HttpRequest& request, std::string& contentTypeDest,
			std::vector<u_char>& bodyDest);
		static int	handleRedirection(const std::string& remainingPath, const RouteConfig::t_redirection& redirection,
			std::string& locationDest);
		static int	handleGet(const std::string& remainingPath, const RouteConfig& routeConfig,
			std::string& contentTypeDest, std::vector<u_char>& bodyDest);
		static int	handlePost(const std::string& remainingPath, const RouteConfig& routeConfig,
			const HttpRequest& request);
		static int	handleDelete(const std::string& remainingPath, const RouteConfig& routeConfig);

		static int			buildError(int errorCode, const ServerConfig& server, std::string& contentTypeDest,
			std::vector<u_char>& bodyDest);
		static std::string	buildErrorPage(int errorCode, const ServerConfig& server);
};

class NotFoundException final : public std::runtime_error {
	public:
		NotFoundException() : std::runtime_error("No route found") {}
};

class MethodNotAllowedException final : public std::runtime_error {
	public:
		MethodNotAllowedException() : std::runtime_error("Method not allowed") {}
};

static const std::string	HTTP_START = "http://";

static const std::string	SAKU_ERROR_PAGE =
	"<!DOCTYPE html>"
	"<html lang=\"en\">"
		"<head>"
			"<meta charset=\"UTF-8\">"
			"<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
			"<title>Saku's Error</title>"
			"<style>"
				"body {"
					"font-family: Arial, sans-serif;"
					"display: flex;"
					"justify-content: center;"
					"align-items: center;"
					"height: 100vh; margin: 0;"
					"background-color: #f0f8ff;"
				"}"
				"h1 {"
					"color: #333;"
				"}"
			"</style>"
		"</head>"
		"<body>"
		"<h1>{{{ERROR}}}</h1>"
		"</body>"
	"</html>";

#endif //HANDLEREQUEST_HPP
