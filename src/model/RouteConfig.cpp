//
// Created by jules on 12/09/2024.
//

#include <RouteConfig.hpp>

bool RouteConfig::operator==(const RouteConfig& other) const {
	return (GET == other.GET &&
				POST == other.POST &&
				DELETE == other.DELETE &&
				location == other.location &&
				listing == other.listing &&
				rootDir == other.rootDir);
}

RouteConfig RouteConfig::fromVariableBlock(std::list<std::string>& routeVariableBlock) {
	const std::string&	location = routeVariableBlock.front();
	std::string			locationBlock = routeVariableBlock.back();

	bool			GET = false;
	bool			POST = false;
	bool			DELETE = false;
	std::string		index;
	bool			listing = false;
	std::string		rootDir;
	t_redirection	redirection = {"", 0};

	try {
		redirection = extractRedirection(locationBlock);
	} catch (Parsing::VariableNotFoundException&) {
		extractMethods(GET, POST, DELETE, locationBlock);
		index = extractIndex(locationBlock);
		listing = extractListing(locationBlock);
		rootDir = extractRootDir(locationBlock);
	}

	if (!IsBlank::isBlank(locationBlock))
		throw ServerConfig::InvalidConfigFileException();

	return (RouteConfig(GET, POST, DELETE, location, index, listing, rootDir, redirection));
}

RouteConfig::RouteConfig(const bool& GET,
	const bool& POST,
	const bool& DELETE,
	const std::string& location,
	const std::string& index,
	const bool& listing,
	const std::string& rootDir,
	const t_redirection& redirection):
	GET(GET),
	POST(POST),
	DELETE(DELETE),
	location(location),
	index(index),
	listing(listing),
	rootDir(rootDir),
	redirection(redirection) { }

RouteConfig::RouteConfig(const RouteConfig& other): GET(other.GET),
													POST(other.POST),
													DELETE(other.DELETE),
													location(other.location),
													index(other.index),
													listing(other.listing),
													rootDir(other.rootDir),
													redirection(other.redirection) {
}

RouteConfig::~RouteConfig() { }

void RouteConfig::extractMethods(bool& GET, bool& POST, bool& DELETE, std::string& locationBlock) {
	std::vector<std::string>	methodsVector = CppSplit::cppSplit(
		Parsing::extractVariable(locationBlock,"methods"), ' ');

	if (methodsVector.empty())
		throw ServerConfig::InvalidConfigFileException();

	if (std::find(methodsVector.cbegin(), methodsVector.cend(), "GET") != std::end(methodsVector))
		GET = true;
	if (std::find(methodsVector.cbegin(), methodsVector.cend(), "POST") != std::end(methodsVector))
		POST = true;
	if (std::find(methodsVector.cbegin(), methodsVector.cend(), "DELETE") != std::end(methodsVector))
		DELETE = true;

	if (static_cast<int>(methodsVector.size()) != static_cast<int>(GET) + static_cast<int>(POST) + static_cast<int>(DELETE))
		throw ServerConfig::InvalidConfigFileException();
}

std::string RouteConfig::extractIndex(std::string& locationBlock) {
	return (Parsing::extractVariable(locationBlock, "index"));
}

bool RouteConfig::extractListing(std::string& locationBlock) {
	std::string	listing	= Parsing::extractVariable(locationBlock, "listing");

	if (listing == "on")
		return (true);
	if (listing == "off")
		return (false);
	throw ServerConfig::InvalidConfigFileException();
}

std::string RouteConfig::extractRootDir(std::string& locationBlock) {
	return (Parsing::extractVariable(locationBlock, "root"));
}

RouteConfig::t_redirection RouteConfig::extractRedirection(std::string& locationBlock) {
	t_redirection				redirection = {"", 0};
	std::string					redirection_string = Parsing::extractVariable(locationBlock, "return");
	std::vector<std::string>	methodsVector = CppSplit::cppSplit(redirection_string, ' ');

	if (methodsVector.size() != 2)
		throw ServerConfig::InvalidConfigFileException();

	redirection.code = StrictUtoi::strictUtoi(methodsVector.front());
	if (redirection.code < 300 || redirection.code > 399)
		throw ServerConfig::InvalidConfigFileException();
	redirection.path = methodsVector.back();

	return (redirection);
}

bool RouteConfig::getGET() const {
    return (GET);
}

bool RouteConfig::getPOST() const {
    return (POST);
}

bool RouteConfig::getDELETE() const {
    return (DELETE);
}

std::string RouteConfig::getLocation() const {
    return (location);
}

std::string RouteConfig::getIndex() const {
	return index;
}

bool RouteConfig::getListing() const {
    return (listing);
}

std::string RouteConfig::getRootDir() const {
    return (rootDir);
}

RouteConfig::t_redirection RouteConfig::getRedirection() const {
	return redirection;
}