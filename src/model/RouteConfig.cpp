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
	std::string	location = routeVariableBlock.front();
	std::string	locationBlock = routeVariableBlock.back();

	bool	GET = false;
	bool	POST = false;
	bool	DELETE = false;

	getMethods(GET, POST, DELETE, locationBlock);

	std::string	rootDir = getRootDir(locationBlock);

	bool listing = getListing(locationBlock);

	if (!IsBlank::isBlank(locationBlock))
		throw ServerConfig::InvalidConfigFileException();

	return (RouteConfig(GET, POST, DELETE, location, listing, rootDir));
}

RouteConfig::RouteConfig(const bool& GET,
	const bool& POST,
	const bool& DELETE,
	const std::string& location,
	const bool& listing,
	const std::string& rootDir):
	GET(GET),
	POST(POST),
	DELETE(DELETE),
	location(location),
	listing(listing),
	rootDir(rootDir) { }

RouteConfig::RouteConfig(const RouteConfig& other): GET(other.GET),
	POST(other.POST),
	DELETE(other.DELETE),
	location(other.location),
	listing(other.listing),
	rootDir(other.rootDir) { }

RouteConfig::~RouteConfig() { }

void RouteConfig::getMethods(bool& GET, bool& POST, bool& DELETE, std::string& locationBlock) {
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

std::string RouteConfig::getRootDir(std::string& locationBlock) {
	return (Parsing::extractVariable(locationBlock, "root"));
}

bool RouteConfig::getListing(std::string& locationBlock) {
	std::string	listing	= Parsing::extractVariable(locationBlock, "listing");

	if (listing == "on")
		return (true);
	if (listing == "off")
		return (false);
	throw ServerConfig::InvalidConfigFileException();
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

bool RouteConfig::getListing() const {
    return (listing);
}

std::string RouteConfig::getRootDir() const {
    return (rootDir);
}