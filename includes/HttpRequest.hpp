//
// Created by jules on 19/12/2024.
//

#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

#include <map>
#include <string>
#include <vector>

#include <ServerConfig.hpp>
#include <StrictUtoi.hpp>
#include <VecBuffCmp.hpp>
#include <http_methods.h>
#include <RequestException.hpp>

static const std::string HTTP_VERSION_STR = "HTTP/1.1";

static const std::string CRLF = "\r\n";
static const std::string HEADER_END_STR = "\r\n\r\n";

typedef enum e_parsing_state {
	PARSING_METHOD,
	PARSING_TARGET,
	PARSING_VERSION,
	PARSING_HEADER,
	PARSING_BODY,
	PARSING_DONE,
}	t_parsing_state;

typedef enum e_request_state {
	REQUEST_PARSING,
	REQUEST_OK,
	REQUEST_INVALID,
	REQUEST_BODY_TOO_LARGE,
	REQUEST_LEN_REQUIRED,
	REQUEST_UNIMPLEMENTED
}	t_request_state;

class HttpRequest {
	private:
		t_request_state		requestState;
		t_parsing_state		parsingState;
		std::vector<u_char>	unparsedData;
		size_t				parseIndex;

		t_method							method;
		std::string							target;
		std::string							version;
		std::map<std::string, std::string>	headers;
		std::vector<u_char>					rawBody;

		ServerConfig*						serverConfig;

	public:
		explicit HttpRequest(ServerConfig* serverConfig);
		HttpRequest(ServerConfig* serverConfig, const u_char* data, size_t len);
		HttpRequest(const HttpRequest &) = default;
		~HttpRequest() = default;

		HttpRequest& operator=(const HttpRequest &) = default;
		bool operator==(const HttpRequest &) const = default;
		bool operator!=(const HttpRequest &) const = default;

		bool	parseData(const u_char* data, size_t len);

		[[nodiscard]] std::vector<u_char>			parseMultiPartFormDataBody(
			std::map<std::string, std::string>& contentDispositionDest) const;

		[[nodiscard]] t_request_state				getRequestState() const;
		[[nodiscard]] e_method						getMethod() const;
		[[nodiscard]] const std::string&			getTarget() const;
		[[nodiscard]] const std::string&			getVersion() const;
		[[nodiscard]] const std::string&			getHeader(const std::string &key) const;
		[[nodiscard]] const std::vector<u_char>&	getBody() const;

		[[nodiscard]] const ServerConfig&			getServerConfig() const;

		static std::map<std::string, std::string>	splitHeaderAttributes(const std::string& headerValue);

	private:
		bool	readParseMethod();
		bool	readParseTarget();
		bool	readParseVersion();
		bool	readParseHeaders();
		bool	readBody();

		void validateHeaders() const;

		[[nodiscard]] bool	exceptSpace(size_t offset) const;

		static bool parseHeaders(const std::vector<u_char>& data, size_t& parseIndex, std::map<std::string, std::string>& dest);

		static bool	isCrlf(const std::vector<u_char>& data, size_t start);
		static bool	isTargetChar(unsigned char c);
		static bool	isHeaderKeyChar(unsigned char c);
		static bool	isHeaderValueChar(unsigned char c);
};

#endif
