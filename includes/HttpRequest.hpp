//
// Created by jules on 19/12/2024.
//

#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

#include <map>
#include <string>
#include <vector>

#include <ServerConfig.hpp>
#include <MultipartFormDataBody.hpp>
#include <RequestBody.hpp>

#include <StrictUtoi.hpp>
#include <VecBuffCmp.hpp>
#include <http_methods.h>
#include <memory>
#include <RequestException.hpp>
#include <SpacesClean.hpp>

static const std::string HTTP_VERSION_STR = "HTTP/1.1";
static const std::string HTTP_STR = "HTTP/";

static const std::string CRLF = "\r\n";
static const std::string HEADER_END_STR = "\r\n\r\n";

static const std::string MULTIPART_FORM_DATA = "multipart/form-data";
static const std::string APPLICATION_OCTET_STREAM = "application/octet-stream";

typedef enum e_request_state {
	REQUEST_PARSING,
	REQUEST_CHUNK_RECEIVING,
	REQUEST_OK,
	REQUEST_INVALID,
	REQUEST_BODY_TOO_LARGE,
	REQUEST_LEN_REQUIRED,
	I_AM_A_TEAPOT,
	REQUEST_UNIMPLEMENTED,
	REQUEST_TIMEOUT,
	HTTP_VERSION_NOT_SUPPORTED,
	SERVER_ERROR
}	t_request_state;

class HttpRequest {
	private:
		typedef enum e_parsing_state {
			PARSING_METHOD,
			PARSING_TARGET,
			PARSING_VERSION,
			PARSING_HEADER,
			PARSING_BODY,
			PARSING_DONE,
		}	t_parsing_state;

		t_request_state		requestState;
		t_parsing_state		parsingState;
		std::vector<u_char>	unparsedData;
		size_t				parseIndex;
		bool				chunked;

		t_method							method;
		std::string							target;
		std::string							version;
		std::map<std::string, std::string>	headers;
		RequestBody*						body;

		ServerConfig*						serverConfig;

		ssize_t								currentChunkSize = -1;

	public:
		explicit HttpRequest(ServerConfig* serverConfig);
		HttpRequest(ServerConfig* serverConfig, const u_char* data, size_t len);
		HttpRequest(const HttpRequest &) = default;
		~HttpRequest();

		HttpRequest& operator=(const HttpRequest &) = default;
		bool operator==(const HttpRequest &) const = default;
		bool operator!=(const HttpRequest &) const = default;

		bool	parseData(const u_char* data, size_t len);
		void	timeout();
		void	serverError();

		[[nodiscard]] bool							isChunked() const;
		[[nodiscard]] t_request_state				getRequestState() const;
		[[nodiscard]] e_method						getMethod() const;
		[[nodiscard]] const std::string&			getTarget() const;
		[[nodiscard]] const std::string&			getVersion() const;
		[[nodiscard]] const std::string&			getHeader(const std::string &key) const;
		[[nodiscard]] RequestBody&					getBody() const;

		[[nodiscard]] const ServerConfig&			getServerConfig() const;

		static bool parseHeaders(const std::vector<u_char>& data, size_t& parseIndex, std::map<std::string, std::string>& dest);

		static std::map<std::string, std::string>	splitHeaderAttributes(const std::string& headerValue);
		static bool	isCrlf(const std::vector<u_char>& data, size_t start);

	private:
		bool	readParseMethod();
		bool	readParseTarget();
		bool	readParseVersion();
		bool	readParseHeaders();
		bool	readBody();
		bool	readChunk();

		void validateHeadersInitBody();

		static bool	isTargetChar(unsigned char c);
		static bool	isHeaderKeyChar(unsigned char c);
};

#endif
