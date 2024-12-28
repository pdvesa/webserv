//
// Created by jules on 27/12/2024.
//

#include <RequestHandler.hpp>

RequestHandler::RequestHandler(const HttpRequest& request) :
	request(request),
	route(nullptr),
	statusCode(0),
	handlingState(STATE_NO)
{ }

bool RequestHandler::handle()
{
	if (handlingState == STATE_YES)
		return (true);
	if (request.getRequestState() == REQUEST_PARSING)
		throw std::runtime_error("Request not parsed");

	try {
		if (request.getRequestState() == REQUEST_OK || request.getRequestState() == REQUEST_CHUNK_RECEIVING)
		{
			if (handlingState == STATE_NO)
			{
				route = parseTarget();
				if (route == nullptr)
					throw NotFoundException();

				if (!route->isMethodAllowed(request.getMethod()))
					throw MethodNotAllowedException();

				if (route->isRedirection())
					handleRedirection();
				else if (request.getMethod() == GET)
					handleGet();
				else if (request.getMethod() == POST)
					handlePost();
				else if (request.getMethod() == DELETE)
					handleDelete();
				else
					throw std::runtime_error("Unknown method");
			}
			else if (handlingState == STATE_WAITING_CHUNK)
			{
				if (request.getMethod() == POST)
					handlePost();
				else
					throw std::runtime_error("Invalid method for chunked request");
			}
		}
		else
		{
			if (!filePartPath.empty())
			{
				try {
					removeFile(filePartPath);
				} catch (...) {
					std::cerr << "Unable to remove temporary file " << filePartPath << std::endl;
				}
				filePartPath.clear();
			}
			handleInvalid();
		}
	} catch (ForbiddenException&) {
		buildError(403);
	}
	catch (NotFoundException&) {
		buildError(404);
	}
	catch (MethodNotAllowedException&) {
		buildError(405);
	}
	catch (InvalidRequestException&) {
		buildError(400);
	}
	catch (NotImplementedException&) {
		buildError(501);
	}
	catch (...) {
		buildError(500);
	}
	return (isHandled());
}

HttpResponse RequestHandler::buildResponse() const
{
	if (!isHandled())
		throw std::runtime_error("Request haven't been handled");
	return (HttpResponse(statusCode, location, contentType, responseBody, request.getMethod()));
}

RouteConfig* RequestHandler::parseTarget()
{
	RouteConfig* routeConfig = nullptr;
	const std::string& requestTarget = request.getTarget();
	const std::string& requestHostname = request.getHeader("Host");
	size_t routeStart = 0;

	if (requestTarget.compare(0, HTTP_START.length(), HTTP_START) == 0)
		routeStart += HTTP_START.size();
	if (requestTarget.compare(0, requestHostname.length(), requestHostname) == 0)
		routeStart += requestHostname.size();

	if (requestTarget[routeStart] == '/' || requestTarget.length() == routeStart)
	{
		if (request.getServerConfig().getRoutes().contains("/"))
			routeConfig = &request.getServerConfig().getRoutes()["/"];
	}
	size_t i = routeStart;
	size_t	routeEnd = routeStart;
	while (i < requestTarget.size())
	{
		const size_t nextSlash = requestTarget.find('/', i);
		if (nextSlash == std::string::npos)
			break ;
		if (std::string route = requestTarget.substr(routeStart, nextSlash - routeStart);
			request.getServerConfig().getRoutes().contains(route)) {
			routeConfig = &request.getServerConfig().getRoutes()[route];
			routeEnd = nextSlash;
		}
		i = nextSlash + 1; // NOLINT(*-narrowing-conversions)
	}
	if (routeEnd + 1 < requestTarget.size())
		remainingPath = requestTarget.substr(routeEnd + 1);
	return (routeConfig);
}

void RequestHandler::handleInvalid()
{
	switch (request.getRequestState()) {
		case REQUEST_INVALID:
			buildError(400);
			break;
		case REQUEST_BODY_TOO_LARGE:
			buildError(413);
			break;
		case REQUEST_LEN_REQUIRED:
			buildError(411);
			break;
		case REQUEST_UNIMPLEMENTED:
			buildError(501);
			break;
		default:
			buildError(500);
	}
}

void RequestHandler::handleRedirection()
{
	location = route->getRedirection().path;
	location += remainingPath;
	statusCode = route->getRedirection().code; // NOLINT(*-narrowing-conversions)
	handlingState = STATE_YES;
}

void RequestHandler::handleGet()
{
	std::string	serverTarget = route->getRootDir();

	if (remainingPath.empty() && !route->getIndex().empty())
		serverTarget += route->getIndex();

	if (access(serverTarget.c_str(), F_OK) != 0)
		throw NotFoundException();
	if (access(serverTarget.c_str(), R_OK) != 0)
		throw ForbiddenException();
	if (std::filesystem::is_regular_file(serverTarget))
	{
		std::ifstream	file(serverTarget, std::ios::binary);
		if (!file.is_open())
			throw std::runtime_error("Unable to open file");
		responseBody = std::vector<u_char>((std::istreambuf_iterator(file)), std::istreambuf_iterator<char>());
		contentType = getContentType(serverTarget);
		file.close();
	}
	else if (std::filesystem::is_directory(serverTarget))
	{
		if (!route->getListing())
			throw ForbiddenException();
		contentType = "text/html";
		std::string listingPage = buildListingPage(serverTarget, request.getTarget());
		responseBody.reserve(listingPage.size());
		responseBody = std::vector<u_char>(listingPage.begin(), listingPage.end());
	}
	else
		throw NotFoundException();
	handlingState = STATE_YES;
	statusCode = 200;
}

void RequestHandler::handlePost()
{
	static std::string	serverTarget = route->getRootDir() + route->getUploadDir() + remainingPath;
	static std::string	contentType = request.getHeader("Content-Type");
	static std::string	filename;

	if (filename.empty())
		filename = getPostUploadFileName();
	if (!filename.empty())
	{
		if (request.isChunked())
		{
			handlingState = STATE_WAITING_CHUNK;
			if (!savePart(serverTarget + filename, request.getBody().getContent(),
				request.getRequestState() == REQUEST_OK))
				return ;
			request.getBody().clearContent();
		}
		else
			saveFile(serverTarget + filename, request.getBody().getContent());
		handlingState = STATE_YES;
		statusCode = 200;
	}
}

void RequestHandler::handleDelete()
{
	const std::string	serverTarget = route->getRootDir() + remainingPath;

	removeFile(serverTarget);
	handlingState = STATE_YES;
	statusCode = 200;
}

std::string RequestHandler::getPostUploadFileName() const
{
	if (contentType.compare(0, MULTIPART_FORM_DATA.length(), MULTIPART_FORM_DATA) == 0)
	{
		const MultipartFormDataBody* body = dynamic_cast<const MultipartFormDataBody*>(&request.getBody());
		if (!body)
			throw std::runtime_error("RequestBody is not of type MultipartFormDataBody");
		try {
			const std::string contentDisposition = body->getHeader("Content-Disposition");
			const std::map<std::string, std::string> attributes = HttpRequest::splitHeaderAttributes(contentDisposition);
			if (!attributes.contains("filename"))
				throw InvalidRequestException();
			return (attributes.at("filename"));
		} catch (std::out_of_range&) {
			if (body->headersDone())
				throw InvalidRequestException();
			return ("");
		}
	}
	if (contentType.compare(0, APPLICATION_OCTET_STREAM.length(), APPLICATION_OCTET_STREAM) == 0)
	{
		try {
			const std::string contentDisposition = request.getHeader("Content-Disposition");
			const std::map<std::string, std::string> attributes =
					HttpRequest::splitHeaderAttributes(contentDisposition);
			return (attributes.at("filename"));
		} catch (std::out_of_range&) {
			throw InvalidRequestException();
		}
	}
	else
		throw NotImplementedException();
}

bool RequestHandler::savePart(const std::string& serverTarget, const std::vector<unsigned char>& data,
	const bool finished)
{
	if (filePartPath.empty() && finished)
	{
		saveFile(serverTarget, data);
		return (true);
	}
	if (filePartPath.empty())
	{
		filePartPath = serverTarget + ".part";
		saveFile(filePartPath, data);
		return (false);;
	}
	appendToFile(filePartPath, data);
	if (finished)
	{
		std::filesystem::rename(filePartPath, serverTarget);
		return (true);
	}
	return (false);
}

void RequestHandler::buildError(const int code)
{
	location.clear();
	contentType = "text/html";
	std::string	errorPage = buildErrorPage(code, request.getServerConfig());
	responseBody.reserve(errorPage.size());
	responseBody = std::vector<u_char>(errorPage.begin(), errorPage.end());
	handlingState = STATE_YES;
	statusCode = code;
}

std::string RequestHandler::buildListingPage(const std::string& serverTarget, const std::string& requestTarget)
{
	std::ostringstream	listingPage;

	errno = 0;
	if (DIR* directory; (directory = opendir(serverTarget.c_str())) != nullptr)
	{
		dirent* entry;
		listingPage << "<html><body><h1>Directory listing for " << requestTarget << "</h1><ul>";
		while ((entry = readdir(directory)) != nullptr)
		{
			if (std::string name = entry->d_name; name != "." && name != "..") {
				listingPage << "<li><a href=\"" << serverTarget << name << "\">" << name << "</a></li>";
			}
		}
		closedir(directory);
		listingPage << "</ul></body></html>";
	} else {
		if (errno == EACCES)
			throw ForbiddenException();
		throw std::runtime_error("Unable to open directory");
	}
	return (listingPage.str());
}

std::string RequestHandler::buildErrorPage(const int errorCode, const ServerConfig& server)
{
	std::string	errorPage;

	if (server.getErrorsPages().contains(errorCode))
		errorPage = server.getErrorsPages()[errorCode];
	else
		errorPage = SAKU_ERROR_PAGE;

	std::string	errorStr;
	int	index;
	try {
		errorStr = std::to_string(errorCode) + " " + httpCodes.at(errorCode);
		index = errorPage.find("{{{ERROR}}}");
		if (index == static_cast<int>(std::string::npos))
			throw std::runtime_error("Error not found");
	} catch (std::out_of_range&) {
		errorPage = SAKU_ERROR_PAGE;
		errorStr = "500 Internal Server Error";
		index = errorPage.find("{{{ERROR}}}");
	} catch (std::runtime_error&) {
		errorPage = SAKU_ERROR_PAGE;
		index = errorPage.find("{{{ERROR}}}");
	}
	errorPage.replace(index, 11, errorStr);
	return (errorPage);
}

void RequestHandler::saveFile(const std::string& filePath, const std::vector<unsigned char>& content)
{
	if (exists(std::filesystem::path(filePath)))
		throw ForbiddenException();
	if (access(filePath.c_str(), W_OK) != 0)
		throw ForbiddenException();
	std::ofstream	file(filePath, std::ios::binary);
	if (!file.is_open())
		throw std::runtime_error("Unable to open file");
	file.write(reinterpret_cast<const char*>(content.data()), content.size());
	file.close();
}

void RequestHandler::appendToFile(const std::string& filePath, const std::vector<unsigned char>& content)
{
	if (!exists(std::filesystem::path(filePath)))
		throw NotFoundException();
	if (is_directory(std::filesystem::path(filePath)))
		throw std::runtime_error("Unable to append to directory");
	if (!is_regular_file(std::filesystem::path(filePath)))
		throw NotFoundException();
	if (access(filePath.c_str(), W_OK) != 0)
		throw ForbiddenException();
	std::ofstream	file(filePath, std::ios::binary | std::ios::app);
	if (!file.is_open())
		throw std::runtime_error("Unable to open file");
	file.write(reinterpret_cast<const char*>(content.data()), content.size());
	file.close();
}

void RequestHandler::removeFile(const std::string& filePath)
{
	if (!exists(std::filesystem::path(filePath)))
		throw NotFoundException();
	if (is_directory(std::filesystem::path(filePath)))
		throw ForbiddenException();
	if (!is_regular_file(std::filesystem::path(filePath)))
		throw NotFoundException();
	if (access(filePath.c_str(), W_OK) != 0)
		throw ForbiddenException();
	if (!std::filesystem::remove(filePath))
		throw std::runtime_error("Unable to remove file");
}

std::string RequestHandler::getContentType(const std::string& filePath)
{
	size_t extensionIndex = filePath.find_last_of('.');
	if (extensionIndex != std::string::npos) {
		std::string extension = filePath.substr(extensionIndex);

		if (extension_to_mime_type.contains(extension))
			return (extension_to_mime_type.at(extension));
	}
	return ("application/octet-stream");
}

bool RequestHandler::isHandled() const
{
	return (handlingState == STATE_YES);
}