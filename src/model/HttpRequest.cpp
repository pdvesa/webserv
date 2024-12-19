//
// Created by jules on 19/12/2024.
//

#include <HttpRequest.hpp>

HttpRequest::HttpRequest() :
	parsingState(PARSING_METHOD),
	method(UNINITIALIZED)
{ }

HttpRequest::HttpRequest(const u_char* data, const size_t len) :
	HttpRequest()
{
	parseData(data, len);
}

bool HttpRequest::parseData(const u_char* data, const size_t len)
{
	try {
		int parsedIndex = 0;

		switch (parsingState)
		{
			case PARSING_METHOD:
				if (!parseMethod(data, len, parsedIndex))
					break ;
				parsingState = PARSING_TARGET;
				[fallthrough];

			case PARSING_TARGET:
				if (!parseTarget(data, len, parsedIndex))
					break ;
				parsingState = PARSING_VERSION;
				[fallthrough];

			case PARSING_VERSION:
				if (!parseVersion(data, len, parsedIndex))
					break ;
				parsingState = PARSING_HEADER;
				[fallthrough];

			case PARSING_HEADER:
				if (!parseHeader(data, len, parsedIndex))
					break ;
				parsingState = PARSING_BODY;
				[fallthrough];

			case PARSING_BODY:
				if (!parseBody(data, len, parsedIndex))
					break ;
				parsingState = PARSING_DONE;
				break ;
			case PARSING_DONE:
				throw InvalidRequest("Request already parsed");
		}

		if (parsedIndex < len - 1)
		{
			if (parsingState == PARSING_DONE)
				throw InvalidRequest("Request already parsed");
			saveUnparsed(data, len, parsedIndex);
		}
		return (true);
	} catch (InvalidRequest&) {
		return (false);
	}

}

void HttpRequest::saveUnparsed(const u_char* data, const size_t len, const int parsedIndex)
{
	for (int i = parsedIndex; i < len; i++)
		unparsedData.push_back(static_cast<u_char>(data[i]));
}

//----GETTERS-----

e_method HttpRequest::getMethod() const {
	return (method);
}

const std::string& HttpRequest::getTarget() const {
	return (target);
}

const std::string& HttpRequest::getVersion() const {
	return (version);
}

const std::string& HttpRequest::getHeader(const std::string& key) const {
	return (headers.at(key));
}

const std::vector<u_char>& HttpRequest::getBody() const {
	return (body);
}

//----GETTERS-----