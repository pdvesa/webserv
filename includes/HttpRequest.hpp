//
// Created by jules on 19/12/2024.
//

#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

#include <map>
#include <string>
#include <vector>

static const std::string GET_STR = "GET ";
static const std::string POST_STR = "POST ";
static const std::string DELETE_STR = "DELETE ";

static const std::string HTTP_VERSION_STR = "HTTP/1.1\n";

static const std::string HEADER_END_STR = "\r\n\r\n";

typedef enum e_method {
	GET = (GET_STR),
	POST = (POST_STR),
	DELETE = (DELETE_STR),
	UNINITIALIZED,
}	t_method;

typedef enum e_parsing_state {
	PARSING_METHOD,
	PARSING_TARGET,
	PARSING_VERSION,
	PARSING_HEADER,
	PARSING_BODY,
	PARSING_DONE,
	PARSING_ERROR,
}	t_parsing_state;

class HttpRequest {
	private:
		t_parsing_state		parsingState;
		std::vector<u_char>	unparsedData;
		size_t				parseIndex;

		t_method							method;
		std::string							target;
		std::string							version;
		std::map<std::string, std::string>	headers;
		std::vector<u_char>					body;

	public:
		HttpRequest();
		HttpRequest(const u_char* data, size_t len);
		HttpRequest(const HttpRequest &) = default;
		~HttpRequest() = default;

		HttpRequest& operator=(const HttpRequest &) = default;
		bool operator==(const HttpRequest &) const = default;
		bool operator!=(const HttpRequest &) const = default;

		bool	parseData(const u_char* data, size_t len);

		[[nodiscard]] e_parsing_state				getParsingState() const;

		[[nodiscard]] e_method						getMethod() const;
		[[nodiscard]] const std::string&			getTarget() const;
		[[nodiscard]] const std::string&			getVersion() const;
		[[nodiscard]] const std::string&			getHeader(const std::string &key) const;
		[[nodiscard]] const std::vector<u_char>&	getBody() const;

		class InvalidRequest final : public std::exception {
			public:
				InvalidRequest() = default;
				explicit InvalidRequest(const char* str);
		};

	private:
		bool	parseMethod();
		bool	parseTarget();
		bool	parseVersion();
		bool	parseHeaders();
		bool	parseBody();

		[[nodiscard]] bool validateHeaders() const;

		static bool	isTargetChar(unsigned char c);
		static bool	isHeaderKeyChar(unsigned char c);
		static bool	isHeaderValueChar(unsigned char c);
};

#endif
