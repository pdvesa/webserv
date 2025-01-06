//
// Created by jules on 27/12/2024.
//

#ifndef REQUESTHANDLER_HPP
# define REQUESTHANDLER_HPP

#include <dirent.h>
#include <filesystem>
#include <bits/fs_fwd.h>
#include <bits/fs_ops.h>
#include <bits/fs_path.h>
#include <unistd.h>

#include <ServerConfig.hpp>
#include <HttpRequest.hpp>
#include <HttpResponse.hpp>
#include <RequestException.hpp>
#include <CGIHandler.hpp>

class RequestHandler {
	private:
		enum e_handling_state {
			STATE_NO,
			STATE_WAITING_CHUNK,
			STATE_YES
		};

		const HttpRequest&	request;
		Client&				client;
		std::string			remainingPath;
		bool				isCgi = false;
		int					pollFD;

		std::string			location;
		std::string			contentType;
		std::vector<u_char>	responseBody;
		int					statusCode;

		e_handling_state	handlingState;

		//POST
		bool				postLog = false;
		std::string			postUploadPath;
		std::string			postUploadFilename;
		bool				filePart = false;


	public:
		explicit RequestHandler(Client &kunde, const HttpRequest& request, int pfd);
		RequestHandler(const RequestHandler&) = delete;
		~RequestHandler() = default;

		RequestHandler	&operator=(const RequestHandler&) = delete;


		bool						handle();
		[[nodiscard]] HttpResponse	buildResponse() const;

		[[nodiscard]] bool			isHandled() const;

		bool						getCGI() {return isCgi;}

	private:
		RouteConfig parseTarget();

		void	handleInvalid();
		void	handleRedirection(const RouteConfig& route);
		void	handleCgi(const RouteConfig& route);
		void	handleGet(const RouteConfig& route);
		void	handlePost(const RouteConfig& route);
		void	handleDelete(const RouteConfig& route);

		void	getPostUploadFilename();

	void savePart(const std::string& serverTarget, const std::string& filename, const std::vector<unsigned char>& data,
				bool finished);

		void	buildError(int code);

		static std::string	buildListingPage(const std::string& serverTarget, const std::string& requestTarget);
		static std::string	buildErrorPage(int errorCode, const ServerConfig& server);

		static void			postLogContent(const std::vector<unsigned char>& content);
		static void			saveFile(const std::string& path, const std::string& filename, const std::vector<unsigned char>& content);
		static void			appendToFile(const std::string& path, const std::string& filename, const std::vector<unsigned char>& content);
		static void			removeFile(const std::string& filePath);

		static std::string	getContentType(const std::string& filePath);
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

#endif
