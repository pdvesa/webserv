//
// Created by jules on 19/12/2024.
//

#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

#include <map>
#include <string>
#include <vector>

#include <HttpResponse.hpp>
#include <ServerConfig.hpp>
#include <StrictUtoi.hpp>
#include <VecBuffCmp.hpp>
#include <http_methods.h>

static const std::string GET_STR = "GET";
static const std::string POST_STR = "POST";
static const std::string DELETE_STR = "DELETE";

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
	REQUEST_LEN_REQUIRED
}	t_request_state;

class HttpResponse;

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
		std::vector<u_char>					body;

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

		[[nodiscard]] HttpResponse					handle() const;

		[[nodiscard]] t_request_state				getRequestState() const;
		[[nodiscard]] e_method						getMethod() const;
		[[nodiscard]] const std::string&			getTarget() const;
		[[nodiscard]] const std::string&			getVersion() const;
		[[nodiscard]] const std::string&			getHeader(const std::string &key) const;
		[[nodiscard]] const std::vector<u_char>&	getBody() const;

		[[nodiscard]] const ServerConfig&			getServerConfig() const;

		class InvalidRequest : public std::exception {
			public:
				InvalidRequest() = default;
				explicit InvalidRequest(const char* str);
		};

		class RequestBodyTooLarge final : public InvalidRequest {
			public:
				RequestBodyTooLarge() = default;
				explicit RequestBodyTooLarge(const char* str);
		};

		class RequestContentLengthMissing final : public InvalidRequest {
			public:
				RequestContentLengthMissing() = default;
				explicit RequestContentLengthMissing(const char* str);
		};

	private:
		bool	parseMethod();
		bool	parseTarget();
		bool	parseVersion();
		bool	parseHeaders();
		bool	parseBody();

		[[nodiscard]] void validateHeaders() const;

		[[nodiscard]] bool	exceptSpace(size_t offset) const;

		[[nodiscard]] bool	isCRLF(size_t offset) const;

		static bool	isTargetChar(unsigned char c);
		static bool	isHeaderKeyChar(unsigned char c);
		static bool	isHeaderValueChar(unsigned char c);
};

#endif
