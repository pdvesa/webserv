//
// Created by jules on 26/12/2024.
//

#ifndef MULTIPARTFORMDATABODY_HPP
# define MULTIPARTFORMDATABODY_HPP

#include <map>
#include <string>

#include <RequestBody.hpp>
#include <HttpRequest.hpp>

#include <VecBuffCmp.hpp>
#include <RequestException.hpp>

class MultipartFormDataBody final : public RequestBody {
	private:
		enum e_parsing_state {
			PARSING_BOUNDARY_START,
			PARSING_BOUNDARY_END,
			PARSING_HEADERS,
			PARSING_CONTENT,
			PARSING_OK,
			PARSING_ERROR
		};
		e_parsing_state		parsingState;
		std::string			boundary;
		std::vector<u_char>	unparsedData;
		size_t				parseIndex;

		std::map<std::string, std::string>	bodyHeaders;
		std::vector<u_char>					bodyContent;


	public:
		explicit MultipartFormDataBody(const std::string& boundary);
		MultipartFormDataBody(const MultipartFormDataBody&) = default;
		~MultipartFormDataBody() override = default;

		MultipartFormDataBody&	operator=(const MultipartFormDataBody&) = default;
		bool					operator==(const MultipartFormDataBody&) const = default;
		bool					operator!=(const MultipartFormDataBody&) const = default;

		bool	addData(const std::vector<u_char>& data) override;

		[[nodiscard]] bool	headersDone() const;

		[[nodiscard]] const std::string&			getHeader(const std::string& key) const;
		[[nodiscard]] const std::vector<u_char>&	getContent() const override;

		void	clearContent() override;

	private:
		bool	readExceptContent(const std::string& content);
		bool	readParseHeaders();
		bool	readContent();
};

#endif
