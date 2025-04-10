//
// Created by jules on 27/12/2024.
//

#include <RequestHandler.hpp>

RequestHandler::RequestHandler(Client& kunde, const HttpRequest& request) :
	request(request),
	client(kunde),
	statusCode(0),
	handlingState(STATE_NO) {
}

bool RequestHandler::handle()
{
	if (handlingState == STATE_YES)
		return (true);
	if (request.getRequestState() == REQUEST_PARSING)
		throw std::runtime_error("Request not parsed");

	try {
		if (request.getRequestState() == REQUEST_OK || request.getRequestState() == REQUEST_CHUNK_RECEIVING)
		{
			const RouteConfig route = parseTarget();
			if (handlingState == STATE_NO)
			{
				if (route.isRedirection())
					handleRedirection(route);
				else if (!route.isMethodAllowed(request.getMethod()))
					throw MethodNotAllowedException();
				else if (isCgi)
					handleCgi(route);
				else if (request.getMethod() == GET)
					handleGet(route);
				else if (request.getMethod() == POST)
					handlePost(route);
				else if (request.getMethod() == DELETE)
					handleDelete(route);
				else
					throw std::runtime_error("Unknown method");
			}
			else if (handlingState == STATE_WAITING_CHUNK)
			{
				if (request.getMethod() == POST)
					handlePost(route);
				else
					throw std::runtime_error("Invalid method for chunked request");
			}
		}
		else
		{
			if (filePart)
			{
				try {
					removeFile(postUploadPath + postUploadFilename + ".part");
				} catch (...) {
					std::cerr << "Unable to remove temporary file " << postUploadPath << postUploadFilename << ".part" << std::endl;
				}
				filePart = false;
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
	catch (IamATeapotException&) {
		buildError(418);
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		buildError(500);
	}
	return (isHandled());
}

std::vector<u_char> RequestHandler::buildResponse() const
{
	if (!isHandled())
		throw std::runtime_error("Request haven't been handled");
//	if (isCgi)
//		return (cgiResponse);
	return (HttpResponse(statusCode, location, contentType, responseBody, request.getMethod()).asResponseBuffer());
}

RouteConfig RequestHandler::parseTarget()
{
	const std::string& requestTarget = request.getTarget();
	const std::string& requestHostname = request.getHeader("Host");
	size_t routeStart = 0;

	if (requestTarget.compare(0, HTTP_START.length(), HTTP_START) == 0) {
		routeStart += HTTP_START.size();
		if (requestTarget.compare(0, requestHostname.length(), requestHostname) == 0)
			routeStart += requestHostname.size();
		else
			throw InvalidRequestException();
	}
	else if (requestTarget.compare(0, requestHostname.length(), requestHostname) == 0)
		routeStart += requestHostname.size();

	RouteConfig routeConfig;
	bool found = false;
	size_t i = routeStart;
	size_t	routeEnd = routeStart;

	while (i < requestTarget.size())
	{
		const size_t nextSlash = requestTarget.find('/', i);
		if (nextSlash == std::string::npos)
			break ;
		std::string route = requestTarget.substr(routeStart, nextSlash - routeStart + 1);
		if (request.getServerConfig().getRoutes().contains(route))
		{
			if (route == "/cgi-bin/" || route == "/cgi-bin")
				isCgi = true;
			else
				isCgi = false;
			found = true;
			routeConfig = request.getServerConfig().getRoutes().at(route);
			routeEnd = nextSlash;
		}
		i = nextSlash + 1; // NOLINT(*-narrowing-conversions)
	}
	if (!found)
		throw NotFoundException();
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
		case REQUEST_TIMEOUT:
			buildError(408);
			break;
		case I_AM_A_TEAPOT:
			buildError(418);
			break;
		case HTTP_VERSION_NOT_SUPPORTED:
			buildError(505);
			break;
		default:
			buildError(500);
	}
}

void RequestHandler::handleRedirection(const RouteConfig& route)
{
	location = route.getRedirection().path;
	location += remainingPath;
	statusCode = route.getRedirection().code; // NOLINT(*-narrowing-conversions)
	handlingState = STATE_YES;
}

void RequestHandler::handleCgi(const RouteConfig& route)
{
	std::string	serverTarget = "." + route.getRootDir() + remainingPath;

	if (remainingPath.empty() && !route.getIndex().empty())
		serverTarget += route.getIndex();

	if (access(serverTarget.c_str(), F_OK) != 0) 
	{
		isCgi = false;
		throw NotFoundException();
	}
	if (std::filesystem::is_regular_file(serverTarget))
	{
		std::filesystem::path path(remainingPath);
		if (path.extension() != ".py" && path.extension() != ".cgi")
		{
			isCgi = false;
			throw NotImplementedException();
		}
		if (access(serverTarget.c_str(), X_OK) != 0) 
		{
			isCgi = false;
			throw ForbiddenException();
		}
		CGI cgiRes(client, request, pollFD, serverTarget);
		if (cgiRes.getCGIStatus() == 1)
			throw std::runtime_error("ok");
	}
	else if (std::filesystem::is_directory(serverTarget))
	{
		isCgi = false;
		if (request.getMethod() != 0)
			throw IamATeapotException();
		if (!route.getListing())
			throw ForbiddenException();
		contentType = "text/html";
		std::string listingPage = buildListingPage(serverTarget, request.getTarget());
		responseBody.reserve(listingPage.size());
		responseBody = std::vector<u_char>(listingPage.begin(), listingPage.end());
	}
	else 
	{
		isCgi = false;
		throw NotFoundException();
	}
	handlingState = STATE_YES;
	statusCode = 200;
}

void RequestHandler::handleGet(const RouteConfig& route)
{
	std::string	serverTarget = "." + route.getRootDir() + remainingPath;

	if (remainingPath.empty() && !route.getIndex().empty())
		serverTarget += route.getIndex();
	
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
		if (!route.getListing())
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

void RequestHandler::handlePost(const RouteConfig& route)
{
	if (postUploadFilename.empty() && !postLog)
	{
		getPostUploadFilename();
		if (!postLog)
		{
			postUploadPath = "." + route.getRootDir();
			if (!exists(std::filesystem::path(postUploadPath + route.getUploadDir())))
			{
				if (access(postUploadPath.c_str(), F_OK) != 0)
					throw NotFoundException();
				if (access(postUploadPath.c_str(), W_OK) != 0)
					throw ForbiddenException();
				if (!std::filesystem::create_directory(postUploadPath + route.getUploadDir()))
					throw std::runtime_error("Unable to create directory");
			}
			postUploadPath += route.getUploadDir() + remainingPath;
			if (postUploadPath.back() != '/')
				postUploadPath += '/';
		}
	}
	if (!postUploadFilename.empty() || postLog)
	{
		if (request.isChunked())
		{
			handlingState = STATE_WAITING_CHUNK;
			if (postLog)
				postLogContent(request.getBody().getContent());
			else
				savePart(postUploadPath, postUploadFilename, request.getBody().getContent(),
			request.getRequestState() == REQUEST_OK);
			request.getBody().clearContent();
			if (request.getRequestState() != REQUEST_OK)
				return ;
		}
		else
		{
			if (postLog)
				postLogContent(request.getBody().getContent());
			else
				saveFile(postUploadPath, postUploadFilename, request.getBody().getContent());
		}
		handlingState = STATE_YES;
		statusCode = 200;
	}
}

void RequestHandler::handleDelete(const RouteConfig& route)
{
	const std::string	serverTarget = "." + route.getRootDir() + remainingPath;

	removeFile(serverTarget);
	handlingState = STATE_YES;
	statusCode = 200;
}

void	RequestHandler::getPostUploadFilename()
{
	if (request.getHeader("Content-Type").compare(0, MULTIPART_FORM_DATA.length(), MULTIPART_FORM_DATA) == 0)
	{
		const MultipartFormDataBody* body = dynamic_cast<const MultipartFormDataBody*>(&request.getBody());
		if (!body)
			throw std::runtime_error("RequestBody is not of type MultipartFormDataBody");
		try {
			const std::string contentDisposition = body->getHeader("Content-Disposition");
			const std::map<std::string, std::string> attributes = HttpRequest::splitHeaderAttributes(contentDisposition);
			if (!attributes.contains("filename"))
				throw InvalidRequestException();
			postUploadFilename = attributes.at("filename");
		} catch (std::out_of_range&) {
			if (body->headersDone())
				postLog = true;
		}
	}
	else if (request.getHeader("Content-Type").compare(0, APPLICATION_OCTET_STREAM.length(), APPLICATION_OCTET_STREAM)
	== 0)
	{
		try {
			const std::string contentDisposition = request.getHeader("Content-Disposition");
			const std::map<std::string, std::string> attributes =
					HttpRequest::splitHeaderAttributes(contentDisposition);
			postUploadFilename = attributes.at("filename");
		} catch (std::out_of_range&) {
			postLog = true;
		}
	}
	else
		postLog = true;
}

void RequestHandler::savePart(const std::string& serverTarget, const std::string& filename,
							const std::vector<unsigned char>& data, const bool finished)
{
	if (!filePart && finished)
		saveFile(serverTarget, filename, data);
	if (!filePart)
	{
		filePart = true;
		saveFile(serverTarget, filename + ".part", data);
	}
	appendToFile(serverTarget, filename + ".part", data);
	if (finished)
		std::filesystem::rename(serverTarget + filename + ".part", serverTarget + filename);
}

void RequestHandler::buildError(const int code)
{
	isCgi = false;
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

	if (DIR* directory; (directory = opendir(serverTarget.c_str())) != nullptr)
	{
		dirent* entry;
		listingPage << "<html><body><h1>Directory listing for " << requestTarget << "</h1><ul>";
		while ((entry = readdir(directory)) != nullptr)
		{
			if (std::string name = entry->d_name; name != "." && name != "..") {
				listingPage << "<li><a href=\"" << requestTarget << (requestTarget.back() == '/' ? "" : "/") << name
				<< "\">" << name << "</a></li>";
			}
		}
		closedir(directory);
		listingPage << "</ul></body></html>";
	} else {
		throw std::runtime_error("Unable to open directory");
	}
	return (listingPage.str());
}

std::string RequestHandler::buildErrorPage(const int errorCode, const ServerConfig& server)
{
	std::string	errorPage;

	if (server.getErrorsPages().contains(errorCode)) {
		try {
			std::vector<unsigned char>	content;
			readFile(server.getErrorsPages().at(errorCode), content);
			errorPage = std::string(content.begin(), content.end());
		} catch (...) { // NOLINT(*-except-type)
			errorPage = SAKU_ERROR_PAGE;
		}
	}
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

void RequestHandler::postLogContent(const std::vector<unsigned char>& content)
{
	for (const unsigned char c : content)
		std::cout << c;
	std::cout << std::endl;
}

void RequestHandler::readFile(const std::string& path, std::vector<unsigned char>& contentDest)
{
	if (access(path.c_str(), F_OK) != 0)
		throw NotFoundException();
	if (access(path.c_str(), R_OK) != 0)
		throw ForbiddenException();
	if (std::filesystem::is_regular_file(path))
	{
		std::ifstream	file(path, std::ios::binary);
		if (!file.is_open())
			throw std::runtime_error("Unable to open file");
		contentDest = std::vector<u_char>((std::istreambuf_iterator(file)), std::istreambuf_iterator<char>());
		file.close();
	}
}


void RequestHandler::saveFile(const std::string& path, const std::string& filename, const std::vector<unsigned char>& content)
{
	if (exists(std::filesystem::path(path + filename)))
		throw ForbiddenException();
	if (access(path.c_str(), W_OK) != 0)
		throw ForbiddenException();
	std::ofstream	file(path + filename, std::ios::binary);
	if (!file.is_open())
		throw std::runtime_error("Unable to open file");
	file.write(reinterpret_cast<const char*>(content.data()), content.size());
	file.close();
}

void RequestHandler::appendToFile(const std::string& path, const std::string& filename, const std::vector<unsigned char>& content)
{
	if (!exists(std::filesystem::path(path + filename)))
		throw NotFoundException();
	if (is_directory(std::filesystem::path(path + filename)))
		throw std::runtime_error("Unable to append to directory");
	if (!is_regular_file(std::filesystem::path(path + filename)))
		throw NotFoundException();
	if (access((path + filename).c_str(), W_OK) != 0)
		throw ForbiddenException();
	std::ofstream	file(path + filename, std::ios::binary | std::ios::app);
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
