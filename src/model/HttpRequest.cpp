//
// Created by jules on 19/12/2024.
//

#include <HttpRequest.hpp>

HttpRequest::HttpRequest(ServerConfig* serverConfig) :
	requestState(REQUEST_PARSING),
	parsingState(PARSING_METHOD),
	parseIndex(0),
	chunked(false),
	method(UNINITIALIZED),
	body(nullptr),
	serverConfig(serverConfig)
{ }

HttpRequest::HttpRequest(ServerConfig* serverConfig, const u_char* data, const size_t len) :
	HttpRequest(serverConfig)
{
	parseData(data, len);
}

HttpRequest::~HttpRequest()
{
	delete body;
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
				validateHeadersInitBody();
				if (method != POST)
				{
					parsingState = PARSING_DONE;
					break ;
				}
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
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		requestState = SERVER_ERROR;
		unparsedData.clear();
		parseIndex = 0;
	}
	return (false);
}

std::map<std::string, std::string> HttpRequest::splitHeaderAttributes(const std::string& headerValue)
{
	std::vector<std::string> attributes = CppSplit::cppSplit(headerValue, ';');

	std::map<std::string, std::string> attributesMap;
	for (std::string& attribute: attributes) {
		const size_t equalIndex = attribute.find('=');
		if (equalIndex == std::string::npos)
			continue ;
		if (equalIndex == 0 || equalIndex == attribute.size() - 1)
			throw InvalidRequestException();

		std::string key = SpacesClean::cleanSpaces(attribute.substr(0, equalIndex));
		std::string value = SpacesClean::cleanSpaces(attribute.substr(equalIndex + 1));

		if (!value.empty() && value.front() == '"' && value.back() == '"')
			value = value.substr(1, value.size() - 2);

		attributesMap.insert(std::make_pair(key, value));
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
	const std::string methodStr = std::string(unparsedData.begin() + parseIndex, // NOLINT(*-narrowing-conversions)
		unparsedData.begin() + parseIndex + spacePos); // NOLINT(*-narrowing-conversions)

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
		if (unparsedData.size() - parseIndex >= HTTP_VERSION_STR.size() + CRLF.length())
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
	if (!body)
		throw (std::runtime_error("Parsing body with body pointer uninitialized"));
	if (!chunked)
	{
		const size_t contentLength = StrictUtoi::strictUtoi(headers.at("Content-Length"));
		if (contentLength > serverConfig->getMaxClientBodySize())
			throw RequestBodyTooLargeException();

		if (contentLength < unparsedData.size() - parseIndex)
			throw InvalidRequestException();

		std::vector<u_char>	bodyData;
		while (parseIndex < unparsedData.size())
			bodyData.push_back(unparsedData[parseIndex++]);

		if (!body->addData(bodyData))
			throw InvalidRequestException();

		if (body->getCumulatedSize() == contentLength)
			return (true);
		return (false);
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

			currentChunkSize = std::stoi(std::string(unparsedData.begin() + parseIndex, // NOLINT(*-narrowing-conversions)
				unparsedData.begin() + endLinePos), nullptr, 16); // NOLINT(*-narrowing-conversions)
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

		std::vector<u_char>	bodyData;
		while (currentChunkSize > 0 && parseIndex < unparsedData.size())
		{
			bodyData.push_back(unparsedData[parseIndex]);
			currentChunkSize--;
			parseIndex++;
		}

		if (!body->addData(bodyData))
			throw InvalidRequestException();

		if (parseIndex >= unparsedData.size() - 1)
			return (false);
		if (!isCrlf(unparsedData, parseIndex))
			throw InvalidRequestException();

		parseIndex += CRLF.length();
		currentChunkSize = -1;
		return (false);
	}
}

void HttpRequest::validateHeadersInitBody()
{
	if (!headers.contains("Host"))
		throw InvalidRequestException();
	if (method == POST)
	{
		if (!headers.contains("Content-Type"))
			throw InvalidRequestException();
		if (headers.contains("Transfer-Encoding") && headers.at("Transfer-Encoding") == "chunked")
		{
			chunked = true;
			requestState = REQUEST_CHUNK_RECEIVING;
		}
		else if (!headers.contains("Content-Length"))
			throw RequestContentLengthMissingException();

		if (const std::string& contentType = headers.at("Content-Type"); contentType.compare(0,
			MULTIPART_FORM_DATA.length(), MULTIPART_FORM_DATA) == 0)
		{
			const std::map<std::string, std::string> attributes = splitHeaderAttributes(contentType);
			if (!attributes.contains("boundary"))
				throw InvalidRequestException();

			body = new MultipartFormDataBody(attributes.at("boundary"));
		}
		else
			body = new RequestBody();
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
			if (!isHeaderValueChar(data[(i + parseIndex)])) {
				std::cerr << "Not a valid header value char"<< data[i + parseIndex] << std::endl;
				throw InvalidRequestException("Not a valid header value char");
			}
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

bool HttpRequest::isHeaderValueChar(const unsigned char c)
{
	return (std::isalnum(c) || std::isspace(c) || c == '-' || c == '_' || c == '.' || c == '!' || c == '~' || c == '*'
		|| c == '\'' || c == '(' || c == ')' || c == '/' || c == ':' || c == '@' || c == '&' || c == '+' || c == '$'
		|| c == ',' || c == ';' || c == '=' || c == '%' || c == '"' || c == '\'');
}

//----GETTERS-----

bool HttpRequest::isChunked() const
{
	return (chunked);
}

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

const ServerConfig& HttpRequest::getServerConfig() const {
	return (*serverConfig);
}

RequestBody& HttpRequest::getBody() const {
	return (*body);
}

//----GETTERS-----
