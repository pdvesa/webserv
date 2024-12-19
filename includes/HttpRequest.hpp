//
// Created by jules on 19/12/2024.
//

#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

#include <map>
#include <string>
#include <vector>

typedef enum e_method {
	GET,
	POST,
	DELETE,
	UNINITIALIZED,
	INVALID //TODO HOW TO HANDLE NOW JUST THROW IMMEDIATELY?
}	t_method;

typedef enum e_parsing_state {
	PARSING_METHOD,
	PARSING_TARGET,
	PARSING_VERSION,
	PARSING_HEADER,
	PARSING_BODY,
	PARSING_DONE
}	t_parsing_state;

class HttpRequest {
	private:
		t_parsing_state		parsingState;
		std::vector<u_char>	unparsedData;

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

		bool	parseData(const u_char* data, size_t len);

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
		bool	parseMethod(const u_char* data, size_t len, int& parsedIndex);
		bool	parseTarget(const u_char* data, size_t len, int& parsedIndex);
		bool	parseVersion(const u_char* data, size_t len, int& parsedIndex);
		bool	parseHeader(const u_char* data, size_t len, int& parsedIndex);
		bool	parseBody(const u_char* data, size_t len, int& parsedIndex);
		void	saveUnparsed(const u_char* data, size_t len, int parsedIndex);
};

#endif
