//
// Created by jules on 26/12/2024.
//

#include <MultipartFormDataBody.hpp>

MultipartFormDataBody::MultipartFormDataBody(const std::string& boundary) :
	parsingState(PARSING_BOUNDARY_START),
	boundary(boundary),
	parseIndex(0)
{ }

bool MultipartFormDataBody::addData(const std::vector<u_char>& data)
{
	unparsedData.insert(unparsedData.end(), data.begin(), data.end());
	try {
		switch (parsingState)
		{
			case PARSING_OK:
				break ;

			case PARSING_BOUNDARY_START:
				parsingState = PARSING_HEADERS;
			if (!readExceptContent("--" + boundary))
				break ;
				//Fallthrough

			case PARSING_HEADERS:
				if (!readParseHeaders())
					break ;
				parsingState = PARSING_CONTENT;
				//Fallthrough

			case PARSING_CONTENT:
				if (!readContent())
					break ;
				parsingState = PARSING_BOUNDARY_END;
				//Fallthrough

			case PARSING_BOUNDARY_END:
				if (!readExceptContent("--" + boundary + "--"))
					break ;
				parsingState = PARSING_OK;
				break ;

			default:
				throw InvalidRequestException();
		}

		unparsedData.erase(unparsedData.begin(), unparsedData.begin() + parseIndex); // NOLINT(*-narrowing-conversions)
		parsedLength += parseIndex;
		parseIndex = 0;

		if (parsingState == PARSING_OK)
		{
			if (!unparsedData.empty())
				throw InvalidRequestException();
		}

		return (true);
	} catch (InvalidRequestException&) {
		parsingState = PARSING_ERROR;
		return (false);
	} catch (std::exception&) {
		parsingState = PARSING_ERROR;
		throw;
	}
}

bool MultipartFormDataBody::readExceptContent(const std::string& content)
{
	if (VecBuffCmp::vecBuffCmp(unparsedData, parseIndex, content.data(), 0,
		std::min(unparsedData.size() - parseIndex, content.size())))
	{
		if (unparsedData.size() - parseIndex >= content.size() + CRLF.length())
		{
			if (!HttpRequest::isCrlf(unparsedData, parseIndex + content.size()))
				throw InvalidRequestException();
			parseIndex += content.size() + CRLF.length();
			return (true);
		}
		return (false);
	}
	throw InvalidRequestException();
}

bool MultipartFormDataBody::readParseHeaders()
{
	return (HttpRequest::parseHeaders(unparsedData, parseIndex, bodyHeaders));
}

bool MultipartFormDataBody::readContent()
{
	while (parseIndex < unparsedData.size())
	{
		if (HttpRequest::isCrlf(unparsedData, parseIndex))
		{
			if (VecBuffCmp::vecBuffCmp(unparsedData, parseIndex, std::string("--" + boundary + "--").c_str(),
				0, std::min(unparsedData.size() - parseIndex, boundary.size() + 4)))
			{
				parseIndex += boundary.size() + 4;
				if (parseIndex < unparsedData.size() && unparsedData.size() - parseIndex <= CRLF.length())
					parseIndex += unparsedData.size() - parseIndex;
				return (true);
			}
		}
		else if (unparsedData.size() - parseIndex < CRLF.length())
			return (false);
		bodyContent.push_back(unparsedData[parseIndex++]);
	}
	return (false);
}

const std::string& MultipartFormDataBody::getHeader(const std::string& key) const
{
	return (this->bodyHeaders.at(key));
}

const std::vector<u_char>& MultipartFormDataBody::getContent() const
{
	return (this->bodyContent);
}

void MultipartFormDataBody::clearContent()
{
	bodyContent.clear();
}

bool MultipartFormDataBody::headersDone() const { //compiling
	return (true);
}