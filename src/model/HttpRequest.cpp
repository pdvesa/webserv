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
				if (!parseMethod())
					break ;
				parsingState = PARSING_TARGET;
				[fallthrough];

			case PARSING_TARGET:
				if (!parseTarget())
					break ;
				parsingState = PARSING_VERSION;
				[fallthrough];

			case PARSING_VERSION:
				if (!parseVersion())
					break ;
				parsingState = PARSING_HEADER;
				[fallthrough];

			case PARSING_HEADER:
				if (!parseHeaders())
					break ;
				validateHeaders();
				if (method != POST)
				{
					parsingState = PARSING_DONE;
					break ;
				}
				parsingState = PARSING_BODY;
				[fallthrough];

			case PARSING_BODY:
				if (!parseBody())
					break ;
				parsingState = PARSING_DONE;
				break ;

			default:
				throw InvalidRequest("Invalid parsing state");
		}

		unparsedData.erase(unparsedData.begin(), unparsedData.begin() + parseIndex); // NOLINT(*-narrowing-conversions)
		parseIndex = 0;

		if (parsingState == PARSING_DONE)
		{
			if (!unparsedData.empty())
				throw InvalidRequest("Invalid parsing state");
			requestState = REQUEST_OK;
		}

		return (true);
	} catch (RequestBodyTooLarge&) {
		requestState = REQUEST_BODY_TOO_LARGE;
		unparsedData.clear();
		parseIndex = 0;
	} catch (RequestContentLengthMissing&) {
		requestState = REQUEST_LEN_REQUIRED;
		unparsedData.clear();
		parseIndex = 0;
	} catch (InvalidRequest&) {
		requestState = REQUEST_INVALID;
		unparsedData.clear();
		parseIndex = 0;
	}
	return (false);
}

bool HttpRequest::parseMethod()
{
	const u_long unparsedSize = unparsedData.size() - parseIndex;

	if (VecBuffCmp::vecBuffCmp(unparsedData, parseIndex, GET_STR.c_str(), 0,
		std::min(GET_STR.size(), unparsedSize)) == 0)
	{
		if (unparsedSize >= GET_STR.size())
		{
			if (!exceptSpace(GET_STR.size()))
				return (false);
			method = GET;
			parseIndex += GET_STR.size() + 1;
			return (true);
		}
		return (false);
	}
	if (VecBuffCmp::vecBuffCmp(unparsedData, parseIndex, POST_STR.c_str(), 0,
		std::min(POST_STR.size(), unparsedSize)) == 0)
	{
		if (unparsedSize >= POST_STR.size())
		{
			if (!exceptSpace(POST_STR.size()))
				return (false);
			method = POST;
			parseIndex += POST_STR.size() + 1;
			return (true);
		}
		return (false);
	}
	if (VecBuffCmp::vecBuffCmp(unparsedData, parseIndex, DELETE_STR.c_str(), 0,
		std::min(DELETE_STR.size(), unparsedSize)) == 0)
	{
		if (unparsedSize >= DELETE_STR.size())
		{
			if (!exceptSpace(DELETE_STR.size()))
				return (false);
			method = DELETE;
			parseIndex += DELETE_STR.size() + 1;
			return (true);
		}
		return (false);
	}
	throw InvalidRequest("Invalid method");
}

bool HttpRequest::parseTarget()
{
	while (parseIndex < unparsedData.size() && unparsedData[parseIndex] != ' ')
	{
		if (!isTargetChar(unparsedData[parseIndex]))
			throw InvalidRequest("Invalid target");
		target += static_cast<char>(unparsedData[parseIndex]);
		parseIndex++;
	}
	if (parseIndex == unparsedData.size())
		return (false);

	if (target.empty())
		throw InvalidRequest("Invalid target");
	parseIndex++;
	return (true);
}

bool HttpRequest::parseVersion()
{
	if (VecBuffCmp::vecBuffCmp(unparsedData, parseIndex, HTTP_VERSION_STR.c_str(), 0,
		std::min(unparsedData.size() - parseIndex, HTTP_VERSION_STR.size())) == 0)
	{
		if (unparsedData.size() - parseIndex >= HTTP_VERSION_STR.size() + (CRLF.length() - 1))
		{
			if (!isCRLF(HTTP_VERSION_STR.size()))
				throw InvalidRequest("Excepted CRLF");
			version = HTTP_VERSION_STR;
			parseIndex += HTTP_VERSION_STR.size() + CRLF.length();
			return (true);
		}
		return (false);
	}
	throw InvalidRequest("Invalid version");
}

bool HttpRequest::parseHeaders()
{
	while (true)
	{
		std::string	key;
		std::string	value;
		int			i = 0;

		if (unparsedData.size() - parseIndex >= 4 &&
			VecBuffCmp::vecBuffCmp(unparsedData, parseIndex, HEADER_END_STR.c_str(), 0,
				HEADER_END_STR.size()) == 0)
		{
			parseIndex += HEADER_END_STR.size();
			return (true);
		}

		while (parseIndex + i < unparsedData.size() && unparsedData[parseIndex + i] != ':')
		{
			if (!isHeaderKeyChar(unparsedData[parseIndex + i]))
				throw InvalidRequest("Invalid header key");
			key += static_cast<char>(unparsedData[parseIndex + i]);
			i++;
		}
		if (parseIndex + i == unparsedData.size())
			return (false);
		if (key.empty())
			throw InvalidRequest("Invalid header key");
		if (headers.contains(key))
			throw InvalidRequest("Duplicate header key");

		i++;
		while (parseIndex + i + (CRLF.length() - 1) < unparsedData.size())
		{
			if (isCRLF(i))
				break ;
			if (!isHeaderValueChar(unparsedData[parseIndex + i]))
				throw InvalidRequest("Invalid header value");
			value += static_cast<char>(unparsedData[parseIndex + i]);
			i++;
		}
		if (parseIndex + i + (CRLF.length() - 1) == unparsedData.size())
			return (false);
		if (value.empty())
			throw InvalidRequest("Invalid header value");
		i += CRLF.length(); // NOLINT(*-narrowing-conversions)

		headers.insert(std::make_pair(key, value));
		parseIndex += i;
	}
}

bool HttpRequest::parseBody()
{
	try {
		static size_t	contentLength = StrictUtoi::strictUtoi(headers.at("Content-Length"));
		if (contentLength > serverConfig->getMaxClientBodySize())
			throw RequestBodyTooLarge("Body too large");

		while (parseIndex < unparsedData.size() && body.size() < contentLength)
			body.push_back(unparsedData[parseIndex++]);

		if (body.size() == contentLength)
			return (true);
		return (false);
	} catch (...) {
		throw InvalidRequest("Invalid body");
	}
}

void HttpRequest::validateHeaders() const {
	if (!headers.contains("Host"))
		throw InvalidRequest("Missing Host header");
	if (method == POST)
	{
		if (!headers.contains("Content-Length")) {
			throw RequestContentLengthMissing("Missing Content-Length header");
		}
		if (!headers.contains("Content-Type"))
			throw InvalidRequest("Missing Content-Type header");
	}
}

bool HttpRequest::exceptSpace(const size_t offset) const
{
	if (parseIndex + offset >= unparsedData.size())
		return (false);
	if (unparsedData[parseIndex + offset] != ' ')
		throw InvalidRequest("Space expected");
	return (true);
}

bool HttpRequest::isCRLF(const size_t offset) const
{
	if (parseIndex + offset + CRLF.length() - 1 >= unparsedData.size())
		return (false);
	if (VecBuffCmp::vecBuffCmp(unparsedData, parseIndex + offset, CRLF.c_str(), 0,
		CRLF.length()) != 0)
		return (false);
	return (true);
}

//TODO Mouais
bool HttpRequest::isTargetChar(const unsigned char c)
{
	if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')
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

//TODO Mouais mouais
bool HttpRequest::isHeaderKeyChar(const unsigned char c)
{
	return (isalnum(c) || c == '-' || c == '_');
}

//TODO Mouais mouais mouais
bool HttpRequest::isHeaderValueChar(const unsigned char c)
{
	return (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '!' || c == '~' || c == '*' || c == '\'' || c == '('
		|| c == ')' || c == '/' || c == ':' || c == '@' || c == '&' || c == '+' || c == '$' || c == ',' || c == ';'
		|| c == '=' || c == '%');
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
	return (body);
}

//----GETTERS-----
