//
// Created by jules on 20/12/2024.
//

#ifndef HANDLEREQUEST_HPP
# define HANDLEREQUEST_HPP

#include <HttpRequest.hpp>
#include <HttpResponse.hpp>
#include <dirent.h>
#include <filesystem>
#include <bits/fs_fwd.h>
#include <bits/fs_ops.h>
#include <bits/fs_path.h>
#include <unistd.h>
#include <RequestException.hpp>
#include <RouteConfig.hpp>
#include <ServerConfig.hpp>

class HttpRequest;
class RouteConfig;
class ServerConfig;

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
							const std::string& requestTarget, std::string& contentTypeDest, std::vector<u_char>& bodyDest);
		static int	handlePost(const std::string& remainingPath, const RouteConfig& routeConfig, const HttpRequest& request);
		static int	handleDelete(const std::string& remainingPath, const RouteConfig& routeConfig);

		static int			buildError(int errorCode, const ServerConfig& server, std::string& contentTypeDest,
			std::vector<u_char>& bodyDest);

		static std::string	buildListingPage(const std::string& serverTarget, const std::string& requestTarget);
		static std::string	buildErrorPage(int errorCode, const ServerConfig& server);

		static int saveFile(const std::string& serverTarget, const std::vector<u_char>& content);

		static std::string							getContentType(const std::string& filePath);
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

static const std::map<std::string, std::string> extension_to_mime_type = {
	{".html", "text/html"},
	{".css", "text/css"},
	{".js", "application/javascript"},
	{".json", "application/json"},
	{".png", "image/png"},
	{".jpg", "image/jpeg"},
	{".jpeg", "image/jpeg"},
	{".gif", "image/gif"},
	{".txt", "text/plain"},
	{".xml", "application/xml"},
	{".svg", "image/svg+xml"},
	{".ico", "image/vnd.microsoft.icon"},
	{".ttf", "font/ttf"},
	{".woff", "font/woff"},
	{".woff2", "font/woff2"},
	{".mp3", "audio/mpeg"},
	{".mp4", "video/mp4"},
	{".pdf", "application/pdf"},
	{".zip", "application/zip"},
	{".rar", "application/vnd.rar"},
	{".csv", "text/csv"},
	{".bmp", "image/bmp"},
	{".webp", "image/webp"},
	{".ogg", "audio/ogg"},
	{".wav", "audio/wav"},
	{".avi", "video/x-msvideo"},
	{".mov", "video/quicktime"},
	{".mpeg", "video/mpeg"},
	{".flv", "video/x-flv"},
	{".wasm", "application/wasm"}
};

#endif //HANDLEREQUEST_HPP
