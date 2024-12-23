//
// Created by jules on 20/12/2024.
//

#include <HandleRequest.hpp>

int HandleRequest::handleRequest(const HttpRequest& request, std::string& locationDest, std::string& contentTypeDest,
								std::vector<u_char>& bodyDest)
{
	if (request.getRequestState() != REQUEST_OK)
		return (handleInvalidRequest(request, contentTypeDest, bodyDest));
	try {
		std::string remainingPath;
		const RouteConfig& routeConfig = parseTarget(request.getTarget(),
			request.getHeader("Host"), request.getServerConfig(), remainingPath);

		if (!routeConfig.isMethodAllowed(request.getMethod()))
			throw MethodNotAllowedException();

		if (routeConfig.isRedirection())
			return (handleRedirection(remainingPath, routeConfig.getRedirection(), locationDest));
		if (request.getMethod() == GET)
			return (handleGet(remainingPath, routeConfig, request.getTarget(), contentTypeDest, bodyDest));
		if (request.getMethod() == POST)
			return (handlePost(remainingPath, routeConfig, request));
		if (request.getMethod() == DELETE)
			return (handleDelete(remainingPath, routeConfig));
		throw MethodNotAllowedException();
	}
	catch (ForbiddenException&) {
		return (buildError(403, request.getServerConfig(), contentTypeDest, bodyDest));
	}
	catch (NotFoundException&) {
		return (buildError(404, request.getServerConfig(), contentTypeDest, bodyDest));
	}
	catch (MethodNotAllowedException&) {
		return (buildError(405, request.getServerConfig(), contentTypeDest, bodyDest));
	}
	catch (InvalidRequestException&) {
		return (buildError(400, request.getServerConfig(), contentTypeDest, bodyDest));
	}
	catch (NotImplementedException&) {
		return (buildError(501, request.getServerConfig(), contentTypeDest, bodyDest));
	}
	catch (...) {
		return (buildError(500, request.getServerConfig(), contentTypeDest, bodyDest));
	}
}

RouteConfig& HandleRequest::parseTarget(const std::string& requestTarget, const std::string& requestHostname,
	const ServerConfig& server, std::string& remainingPathDest)
{
	RouteConfig* routeConfig = nullptr;
	size_t routeStart = 0;

	if (requestTarget.compare(HTTP_START) == 0)
		routeStart += HTTP_START.size();
	if (requestTarget.compare(requestHostname) == 0)
		routeStart += requestHostname.size();

	if (requestTarget[routeStart] == '/' || requestTarget.length() == routeStart)
	{
		if (server.getRoutes().contains("/"))
			routeConfig = &server.getRoutes()["/"];
	}
	size_t i = routeStart;
	size_t	routeEnd = routeStart;
	while (i < requestTarget.size())
	{
		const size_t nextSlash = requestTarget.find('/', i);
		if (nextSlash == std::string::npos)
			break ;
		if (std::string route = requestTarget.substr(routeStart, nextSlash - routeStart); server.getRoutes().contains(route)) {
			routeConfig = &server.getRoutes()[route];
			routeEnd = nextSlash;
		}
		i = nextSlash + 1; // NOLINT(*-narrowing-conversions)
	}
	if (routeConfig == nullptr)
		throw NotFoundException();
	if (routeEnd + 1 < requestTarget.size())
		remainingPathDest = requestTarget.substr(routeEnd + 1);
	return (*routeConfig);
}

int HandleRequest::handleInvalidRequest(const HttpRequest& request, std::string& contentTypeDest,
	std::vector<u_char>& bodyDest)
{
	switch (request.getRequestState()) {
		case REQUEST_INVALID:
			return (buildError(400, request.getServerConfig(), contentTypeDest, bodyDest));
		case REQUEST_BODY_TOO_LARGE:
			return (buildError(413, request.getServerConfig(), contentTypeDest, bodyDest));
		case REQUEST_LEN_REQUIRED:
			return (buildError(411, request.getServerConfig(), contentTypeDest, bodyDest));
		case REQUEST_UNIMPLEMENTED:
			return (buildError(501, request.getServerConfig(), contentTypeDest, bodyDest));
		default:
			return (buildError(500, request.getServerConfig(), contentTypeDest, bodyDest));
	}
}

int HandleRequest::handleRedirection(const std::string& remainingPath, const RouteConfig::t_redirection& redirection,
	std::string& locationDest)
{
	locationDest = redirection.path;
	locationDest += remainingPath;
	return (redirection.code);
}

int HandleRequest::handleGet(const std::string& remainingPath, const RouteConfig& routeConfig,
	const std::string& requestTarget, std::string& contentTypeDest, std::vector<u_char>& bodyDest)
{
	std::string	serverTarget = routeConfig.getRootDir();
	if (remainingPath.empty() && !routeConfig.getIndex().empty())
		serverTarget += routeConfig.getIndex();

	if (access(serverTarget.c_str(), F_OK) != 0)
		throw NotFoundException();
	if (access(serverTarget.c_str(), R_OK) != 0)
		throw ForbiddenException();
	if (std::filesystem::is_regular_file(serverTarget))
	{
		std::ifstream	file(serverTarget, std::ios::binary);
		if (!file.is_open())
			throw std::runtime_error("Unable to open file");
		//TODO Check some size and optimize ???
		bodyDest = std::vector<u_char>((std::istreambuf_iterator(file)), std::istreambuf_iterator<char>());
		contentTypeDest = getContentType(serverTarget);
		file.close();
	}
	else if (std::filesystem::is_directory(serverTarget))
	{
		if (!routeConfig.getListing())
			throw ForbiddenException();
		contentTypeDest = "text/html";
		std::string responseBody = buildListingPage(serverTarget, requestTarget);
		bodyDest = std::vector<u_char>(responseBody.begin(), responseBody.end());
	}
	else
		throw NotFoundException();
	return (200);
}

int HandleRequest::handlePost(const std::string& remainingPath, const RouteConfig& routeConfig,
	const HttpRequest& request)
{
	std::string	serverTarget = routeConfig.getRootDir() + routeConfig.getUploadDir() + remainingPath;
	std::vector<u_char>	fileContent;
	std::string	contentType = request.getHeader("Content-Type");

	if (contentType.compare("multipart/form-data;") == 0)
	{
		std::map<std::string, std::string>	contentDisposition;
		fileContent = request.parseMultiPartFormDataBody(contentDisposition);
		if (!contentDisposition.contains("filename"))
			throw InvalidRequestException();
		serverTarget += contentDisposition["filename"];
	}
	else if (contentType.compare("application/octet-stream") == 0)
	{
		try {
			std::string contentDisposition = request.getHeader("Content-Disposition");
			std::map<std::string, std::string> attributes = HttpRequest::splitHeaderAttributes(contentDisposition);
			if (!attributes.contains("filename"))
				throw InvalidRequestException();
			fileContent = request.getBody();
			serverTarget += attributes["filename"];
		} catch (std::out_of_range&) {
			throw InvalidRequestException();
		}
	}
	else
		throw NotImplementedException();
	return (saveFile(serverTarget, fileContent));
}

int HandleRequest::handleDelete(const std::string& remainingPath, const RouteConfig& routeConfig)
{
	std::string	serverTarget = routeConfig.getRootDir() + remainingPath;

	if (!exists(std::filesystem::path(serverTarget)))
		throw NotFoundException();
	if (is_directory(std::filesystem::path(serverTarget)))
		throw ForbiddenException();
	if (!is_regular_file(std::filesystem::path(serverTarget)))
		throw NotFoundException();
	if (access(serverTarget.c_str(), W_OK) != 0)//TODO permission
		throw ForbiddenException();
	if (!std::filesystem::remove(serverTarget))
		throw std::runtime_error("Unable to remove file");
	return (200);
}

int HandleRequest::buildError(const int errorCode, const ServerConfig& server, std::string& contentTypeDest,
	std::vector<u_char>& bodyDest)
{
	contentTypeDest = "text/html";
	bodyDest = std::vector<u_char>(buildErrorPage(errorCode, server).begin(),
		buildErrorPage(errorCode, server).end());
	return (errorCode);
}

std::string HandleRequest::buildListingPage(const std::string& serverTarget, const std::string& requestTarget)
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

std::string HandleRequest::buildErrorPage(const int errorCode, const ServerConfig& server)
{
	std::string	errorPage;

	if (server.getErrorsPages().contains(errorCode))
		errorPage = server.getErrorsPages()[errorCode];
	else
		errorPage = SAKU_ERROR_PAGE;

	std::string	errorStr;
	int	index;
	try {
		errorStr = std::to_string(errorCode) + " " + httpErrors.at(errorCode);
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

int HandleRequest::saveFile(const std::string& serverTarget, const std::vector<u_char>& content)
{
	if (exists(std::filesystem::path(serverTarget)))
		throw ForbiddenException();
	if (access(serverTarget.c_str(), W_OK) != 0)
		throw ForbiddenException();
	std::ofstream	file(serverTarget, std::ios::binary);
	if (!file.is_open())
		throw std::runtime_error("Unable to open file");
	file.write(reinterpret_cast<const char*>(content.data()), content.size());
	file.close();
	return (200);
}

std::string HandleRequest::getContentType(const std::string& filePath)
{
	size_t extensionIndex = filePath.find_last_of('.');
	if (extensionIndex != std::string::npos) {
		std::string extension = filePath.substr(extensionIndex);

		if (extension_to_mime_type.contains(extension))
			return (extension_to_mime_type.at(extension));
	}
	return ("application/octet-stream");
}
