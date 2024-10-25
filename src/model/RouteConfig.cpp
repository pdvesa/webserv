//
// Created by jules on 12/09/2024.
//

#include <RouteConfig.hpp>

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
