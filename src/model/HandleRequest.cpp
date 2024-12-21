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
			return (handleGet(remainingPath, routeConfig, contentTypeDest, bodyDest));
		if (request.getMethod() == POST)
			return (handlePost(remainingPath, routeConfig, request));
		if (request.getMethod() == DELETE)
			return (handleDelete(remainingPath, routeConfig));
		throw MethodNotAllowedException();
	}
	catch (NotFoundException&) {
		return (buildError(404, request.getServerConfig(), contentTypeDest, bodyDest));
	}
	catch (MethodNotAllowedException&) {
		return (buildError(405, request.getServerConfig(), contentTypeDest, bodyDest));
	}
	catch (...) {
		return (buildError(500, request.getServerConfig(), contentTypeDest, bodyDest));
	}
}

RouteConfig& HandleRequest::parseTarget(const std::string& requestTarget, const std::string& requestHostname,
										const ServerConfig& server, std::string& remainingPathDest)
{
	RouteConfig* routeConfig = nullptr;
	int routeStart = 0;

	if (requestTarget.compare(HTTP_START) == 0)
		routeStart += HTTP_START.size();
	if (requestTarget.compare(requestHostname) == 0)
		routeStart += requestHostname.size();

	if (requestTarget[routeStart] == '/' || requestTarget.length() == routeStart)
	{
		if (server.getRoutes().contains("/"))
			routeConfig = &server.getRoutes()["/"];
	}
	int i = routeStart;
	int	routeEnd = routeStart;
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
		default:
			return (buildError(500, request.getServerConfig(), contentTypeDest, bodyDest));
	}
}

int HandleRequest::buildError(const int errorCode, const ServerConfig& server, std::string& contentTypeDest,
	std::vector<u_char>& bodyDest)
{
	contentTypeDest = "text/html";
	bodyDest = std::vector<u_char>(buildErrorPage(errorCode, server).begin(),
		buildErrorPage(errorCode, server).end());
	return (errorCode);
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

int HandleRequest::handleRedirection(const std::string& remainingPath, const RouteConfig::t_redirection& redirection,
	std::string& locationDest)
{
	locationDest = redirection.path;
	locationDest += remainingPath;
	return (redirection.code);
}
