//
// Created by jules on 19/12/2024.
//

#include <HttpRequest.hpp>

HttpRequest::HttpRequest(ServerConfig* serverConfig) :
	requestState(REQUEST_PARSING),
	parsingState(PARSING_METHOD),
	parseIndex(0),
	method(UNINITIALIZED),
	serverConfig(serverConfig)
{ }

HttpRequest::HttpRequest(ServerConfig* serverConfig, const u_char* data, const size_t len) :
	HttpRequest(serverConfig)
{
	parseData(data, len);
}

bool HttpRequest::parseData(const u_char* data, const size_t len)
{
	if (requestState != REQUEST_PARSING)
		return (false);
	try {
		unparsedData.insert(unparsedData.end(), data, data + len);

		switch (parsingState)
		{
			case PARSING_DONE:
				break ;

			case PARSING_METHOD:
				if (!readParseMethod())
					break ;
				parsingState = PARSING_TARGET;
				//Fallthrough

			case PARSING_TARGET:
				if (!readParseTarget())
					break ;
				parsingState = PARSING_VERSION;
				//Fallthrough

			case PARSING_VERSION:
				if (!readParseVersion())
					break ;
				parsingState = PARSING_HEADER;
				//Fallthrough

			case PARSING_HEADER:
				if (!readParseHeaders())
					break ;
				validateHeaders();
				if (method != POST)
				{
					parsingState = PARSING_DONE;
					break ;
				}
				requestState = REQUEST_BODY_PARSING;
				parsingState = PARSING_BODY;
				//Fallthrough

			case PARSING_BODY:
				if (!readBody())
					break ;
				parsingState = PARSING_DONE;
				break ;

			default:
				throw InvalidRequestException();
		}

		unparsedData.erase(unparsedData.begin(), unparsedData.begin() + parseIndex); // NOLINT(*-narrowing-conversions)
		parseIndex = 0;

		if (parsingState == PARSING_DONE)
		{
			if (!unparsedData.empty())
				throw InvalidRequestException();
			requestState = REQUEST_OK;
		}

		return (true);
	} catch (NotImplementedException&) {
		requestState = REQUEST_UNIMPLEMENTED;
		unparsedData.clear();
		parseIndex = 0;
	} catch (RequestBodyTooLargeException&) {
		requestState = REQUEST_BODY_TOO_LARGE;
		unparsedData.clear();
		parseIndex = 0;
	} catch (RequestContentLengthMissingException&) {
		requestState = REQUEST_LEN_REQUIRED;
		unparsedData.clear();
		parseIndex = 0;
	} catch (InvalidRequestException& e) {
		std::cerr << e.what() << std::endl;
		requestState = REQUEST_INVALID;
		unparsedData.clear();
		parseIndex = 0;
	}
	return (false);
}

std::vector<u_char> HttpRequest::parseMultiPartFormDataBody(std::map<std::string, std::string>& contentDispositionDest)
	const
{
	if (requestState != REQUEST_OK)
		throw std::runtime_error("Invalid request state");
	const std::string	contentType = headers.at("Content-Type");
	if (contentType != "multipart/form-data;")
		throw std::runtime_error("Server shouldn't call this code with that");

	std::map<std::string, std::string> attributes = splitHeaderAttributes(contentType);
	if (!attributes.contains("boundary"))
		throw InvalidRequestException();
	const std::string boundary = attributes["boundary"];
	if (boundary.size() < 2)
		throw InvalidRequestException();

	size_t contentIndex = 0;
	if (VecBuffCmp::vecBuffCmp(rawBody, contentIndex, (std::string("--") + boundary).c_str()
		, 0, boundary.size() + 2) != 0)
		throw InvalidRequestException();
	contentIndex += (2 + boundary.size());
	if (!isCrlf(rawBody, contentIndex))
		throw InvalidRequestException();
	contentIndex += CRLF.length();

	const ssize_t endIndex = static_cast<ssize_t>(rawBody.size()) - boundary.size() - 4; // NOLINT(*-narrowing-conversions)
	if (endIndex < 0)
		throw InvalidRequestException();
	if (VecBuffCmp::vecBuffCmp(rawBody, endIndex, (std::string("--") + boundary +
		std::string("--")).c_str(), 0, boundary.size() + 4) != 0)
		throw InvalidRequestException();

	std::map<std::string, std::string>	bodyHeaders;
	if (!parseHeaders(rawBody, contentIndex, bodyHeaders))
		throw InvalidRequestException();

	if (!bodyHeaders.contains(std::string("Content-Disposition")))
		throw InvalidRequestException();

	contentDispositionDest = splitHeaderAttributes(bodyHeaders.at(std::string("Content-Disposition")));
	if (!contentDispositionDest.contains(std::string("filename")))
		throw InvalidRequestException();

	while (contentIndex < endIndex - HEADER_END_STR.size())
	{
		if (VecBuffCmp::vecBuffCmp(rawBody, contentIndex, HEADER_END_STR.c_str(),
			0, HEADER_END_STR.size()) == 0)
		{
			contentIndex += HEADER_END_STR.size();
			break;
		}
		contentIndex++;
	}
	if (static_cast<ssize_t>(contentIndex) == endIndex)
		throw InvalidRequestException();

	std::vector<u_char>	bodyContent;
	while (static_cast<ssize_t>(contentIndex) < endIndex)
	{
		bodyContent.push_back(rawBody[contentIndex]);
		contentIndex++;
	}
	return (bodyContent);
}

//TODO Really bad not actual parsing of "" or escaped char to improve GREATLY :P
std::map<std::string, std::string> HttpRequest::splitHeaderAttributes(const std::string& headerValue)
{
	std::vector<std::string>	attributes = CppSplit::cppSplit(headerValue, ';');

	std::map<std::string, std::string>	attributesMap;
	for (const std::string& attribute : attributes)
	{
		const size_t equalIndex = attribute.find('=');
		if (equalIndex == std::string::npos || equalIndex == 0 || equalIndex == attribute.size() - 1)
			throw InvalidRequestException();
		attributesMap.insert(std::make_pair(attribute.substr(0, equalIndex),
			attribute.substr(equalIndex + 1)));
	}
	return (attributesMap);
}

bool HttpRequest::readParseMethod()
{
	size_t spacePos = 0;

	while (spacePos < unparsedData.size() - parseIndex)
	{
		if (unparsedData[parseIndex + spacePos] == ' ')
			break ;
		if (!isalpha(unparsedData[parseIndex + spacePos]) || spacePos > 6)
			throw InvalidRequestException();
		spacePos++;
	}
	if (unparsedData[parseIndex + spacePos] != ' ')
		return (false);
	const std::string methodStr = std::string(unparsedData.begin() + parseIndex,
		unparsedData.begin() + parseIndex + spacePos);

	method = stringToMethod(methodStr);
	if (method == UNINITIALIZED)
		throw NotImplementedException();

	parseIndex += spacePos + 1;
	return (true);
}

bool HttpRequest::readParseTarget()
{
	while (parseIndex < unparsedData.size() && unparsedData[parseIndex] != ' ')
	{
		if (!isTargetChar(unparsedData[parseIndex]))
			throw InvalidRequestException();
		target += static_cast<char>(unparsedData[parseIndex]);
		parseIndex++;
	}
	if (parseIndex == unparsedData.size())
		return (false);

	if (target.empty())
		throw InvalidRequestException();
	parseIndex++;
	return (true);
}

bool HttpRequest::readParseVersion()
{
	if (VecBuffCmp::vecBuffCmp(unparsedData, parseIndex, HTTP_VERSION_STR.c_str(), 0,
		std::min(unparsedData.size() - parseIndex, HTTP_VERSION_STR.size())) == 0)
	{
		if (unparsedData.size() - parseIndex >= HTTP_VERSION_STR.size() + (CRLF.length() - 1))
		{
			if (!isCrlf(unparsedData, parseIndex + HTTP_VERSION_STR.size())) {
				throw InvalidRequestException();
			}
			version = HTTP_VERSION_STR;
			parseIndex += HTTP_VERSION_STR.size() + CRLF.length();
			return (true);
		}
		return (false);
	}
	throw InvalidRequestException();
}

bool HttpRequest::readParseHeaders()
{
	return (parseHeaders(unparsedData, parseIndex, headers));
}

bool HttpRequest::readBody()
{
	if (!chunked)
	{
		try {
			static size_t	contentLength = StrictUtoi::strictUtoi(headers.at("Content-Length"));
			if (contentLength > serverConfig->getMaxClientBodySize())
				throw RequestBodyTooLargeException();

			while (parseIndex < unparsedData.size() && rawBody.size() < contentLength)
				rawBody.push_back(unparsedData[parseIndex++]);

			if (rawBody.size() == contentLength)
				return (true);
			return (false);
		} catch (...) {
			throw InvalidRequestException();
		}
	}
	else
		return (readChunk());
}

bool HttpRequest::readChunk()
{
	while (true)
	{
		static ssize_t	currentChunkSize = -1;
		if (currentChunkSize == -1)
		{
			size_t	endLinePos = parseIndex;
			while (endLinePos < unparsedData.size() - 1)
			{
				if (isCrlf(unparsedData, endLinePos))
					break ;
				if (!isdigit(unparsedData[endLinePos]) && !islower(unparsedData[endLinePos]))
					throw InvalidRequestException();
				endLinePos++;
			}
			if (endLinePos == unparsedData.size() - 1)
				return (false);

			currentChunkSize = std::stoi(std::string(unparsedData.begin() + parseIndex,
				unparsedData.begin() + endLinePos), nullptr, 16);
			if (currentChunkSize > serverConfig->getMaxClientBodySize())
				throw RequestBodyTooLargeException();

			if (currentChunkSize == 0)
			{
				if (endLinePos + CRLF.length() + CRLF.length() >= unparsedData.size())
				{
					if (!isCrlf(unparsedData, endLinePos + CRLF.length()))
						throw InvalidRequestException();
					parseIndex = endLinePos + CRLF.length() + CRLF.length();
					return (true);
				}
				else
				{
					currentChunkSize = - 1;
					return (false);
				}
			}
			else
				parseIndex = endLinePos + CRLF.length();
		}

		while (currentChunkSize > 0 && parseIndex < unparsedData.size())
		{
			rawBody.push_back(unparsedData[parseIndex]);
			currentChunkSize--;
			parseIndex++;
		}
		if (parseIndex >= unparsedData.size() - 1)
			return (false);
		if (!isCrlf(unparsedData, parseIndex))
			throw InvalidRequestException();

		parseIndex += CRLF.length();
		currentChunkSize = -1;
		return (false);
	}
}

void HttpRequest::validateHeaders()
{
	if (!headers.contains("Host"))
		throw InvalidRequestException();
	if (method == POST)
	{
		if (!headers.contains("Content-Type"))
			throw InvalidRequestException();
		if (!headers.contains("Content-Length")) {
			if (!headers.contains("Transfer-Encoding") || headers.at("Transfer-Encoding") != "chunked")
				throw RequestContentLengthMissingException();
			chunked = true;
		}
	}
}

bool HttpRequest::parseHeaders(const std::vector<u_char>& data, size_t& parseIndex, std::map<std::string,
	std::string>& dest)
{
	while (true)
	{
		size_t	i = 0;
		std::string	key;
		std::string	value;

		if (data.size() - parseIndex >= 2 &&
			VecBuffCmp::vecBuffCmp(data, parseIndex, CRLF.c_str(), 0,
				CRLF.size()) == 0)
		{
			parseIndex += CRLF.size();
			return (true);
		}

		while ((i + parseIndex) < data.size() && data[(i + parseIndex)] != ':')
		{
			if (!isHeaderKeyChar(data[(i + parseIndex)]))
				throw InvalidRequestException("Not a valid header key char");
			key += static_cast<char>(data[(i + parseIndex)]);
			i++;
		}
		if ((i + parseIndex) == data.size())
			return (false);
		if (key.empty())
			throw InvalidRequestException("Empty header key");
		if (dest.contains(key))
			throw InvalidRequestException("Duplicate header key");

		i++;
		while ((i + parseIndex) + (CRLF.length() - 1) < data.size())
		{
			if (isCrlf(data, (i + parseIndex)))
				break ;
			if (!isHeaderValueChar(data[(i + parseIndex)]))
				throw InvalidRequestException();
			value += static_cast<char>(data[(i + parseIndex)]);
			i++;
		}
		if ((i + parseIndex) + (CRLF.length() - 1) == data.size())
			return (false);
		value = SpacesClean::cleanSpaces(value);
		if (value.empty())
			throw InvalidRequestException("Empty header value");
		i += CRLF.length(); // NOLINT(*-narrowing-conversions)

		dest.insert(std::make_pair(key, value));
		parseIndex += i;
	}
}


bool HttpRequest::isCrlf(const std::vector<u_char>& data, const size_t start)
{
	if (data.size() - start < CRLF.length())
		return (false);
	if (VecBuffCmp::vecBuffCmp(data, start, CRLF.c_str(), 0, CRLF.length()) != 0)
		return (false);
	return (true);
}

//TODO Mouais unimplemented
bool HttpRequest::isTargetChar(const unsigned char c)
{
	if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')
		return (true);
	if (c == ':' || c == '/' || c == '?' || c == '#' || c == '[' || c == ']' || c == '@')
		return (true);
	if (c == '%')
		return (true);
	if (c == '!' || c == '$' || c == '&' || c == '\'' || c == '(' || c == ')' || c == '*' || c == '+' || c == ','
		|| c == ';' || c == '=')
		return (true);
	return (false);
}

bool HttpRequest::isHeaderKeyChar(const unsigned char c)
{
	return (std::isalnum(c) || c == '-' || c == '_');
}

//TODO Mouais mouais mouais
bool HttpRequest::isHeaderValueChar(const unsigned char c)
{
	return (std::isalnum(c) || std::isspace(c) || c == '-' || c == '_' || c == '.' || c == '!' || c == '~' || c == '*'
		|| c == '\'' || c == '(' || c == ')' || c == '/' || c == ':' || c == '@' || c == '&' || c == '+' || c == '$'
		|| c == ',' || c == ';' || c == '=' || c == '%');
}

//----GETTERS-----

t_request_state HttpRequest::getRequestState() const {
	return (requestState);
}

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
	return (rawBody);
}

const ServerConfig& HttpRequest::getServerConfig() const {
	return (*serverConfig);
}

//----GETTERS-----
