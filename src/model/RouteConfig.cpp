//
// Created by jules on 12/09/2024.
//

#include <RouteConfig.hpp>

RouteConfig RouteConfig::fromLocationBlock(std::string& locationBlock) {

	bool			GET = false;
	bool			POST = false;
	bool			DELETE = false;
	std::string		index = "";
	bool			listing = false;
	std::string		rootDir;
	std::string		uploadDir;
	t_redirection	redirection = {"", 0};

	try {
		redirection = extractRedirection(locationBlock);
	} catch (Parsing::VariableNotFoundException&) {
		try {
			index = extractIndex(locationBlock);
		} catch (Parsing::VariableNotFoundException&) { }
		extractMethods(GET, POST, DELETE, locationBlock);
		listing = extractListing(locationBlock);
		rootDir = extractRootDir(locationBlock);
		if (POST) {
			try {
				uploadDir = extractUploadDir(locationBlock);
			} catch (Parsing::VariableNotFoundException&) { }
		}
	}

	if (!IsBlank::isBlank(locationBlock))
		throw InvalidConfigFileException();

	return (RouteConfig(GET, POST, DELETE, index, listing, rootDir, uploadDir, redirection));
}

bool RouteConfig::isRedirection() const
{
	return (redirection.code != 0);
}

bool RouteConfig::isMethodAllowed(const t_method method) const
{
	switch (method) {
		case e_method::GET:
			return (GET);
		case e_method::POST:
			return (POST);
		case e_method::DELETE:
			return (DELETE);
		default:
			throw std::runtime_error("Invalid method");
	}
}

RouteConfig::RouteConfig(const bool& GET,
						const bool& POST,
						const bool& DELETE,
						const std::string& index,
						const bool& listing,
						const std::string& rootDir,
						const std::string& uploadDir,
						const t_redirection& redirection):
	GET(GET),
	POST(POST),
	DELETE(DELETE),
	index(index),
	listing(listing),
	rootDir(rootDir),
	uploadDir(uploadDir),
	redirection(redirection) { }

RouteConfig::RouteConfig(const RouteConfig& other): GET(other.GET),
													POST(other.POST),
													DELETE(other.DELETE),
													index(other.index),
													listing(other.listing),
													rootDir(other.rootDir),
													uploadDir(other.uploadDir),
													redirection(other.redirection) {
}

RouteConfig::~RouteConfig() { }

RouteConfig& RouteConfig::operator=(const RouteConfig& other) {
	if (this != &other) {
		this->GET = other.GET;
		this->POST = other.POST;
		this->DELETE = other.DELETE;
		this->index = other.index;
		this->listing = other.listing;
		this->rootDir = other.rootDir;
		this->uploadDir = other.uploadDir;
		this->redirection = other.redirection;
	}
	return (*this);
}

bool RouteConfig::operator==(const RouteConfig& other) const {
	return (GET == other.GET &&
				POST == other.POST &&
				DELETE == other.DELETE &&
				index == other.index &&
				listing == other.listing &&
				rootDir == other.rootDir &&
				uploadDir == other.uploadDir &&
				redirection.code == other.redirection.code &&
				redirection.path == other.redirection.path);
}

void RouteConfig::extractMethods(bool& GET, bool& POST, bool& DELETE, std::string& locationBlock) {
	std::vector<std::string>	methodsVector = CppSplit::cppSplit(
		Parsing::extractVariable(locationBlock,"methods"), ' ');

	if (methodsVector.empty())
		throw InvalidConfigFileException();

	if (std::find(methodsVector.cbegin(), methodsVector.cend(), "GET") != std::end(methodsVector))
		GET = true;
	if (std::find(methodsVector.cbegin(), methodsVector.cend(), "POST") != std::end(methodsVector))
		POST = true;
	if (std::find(methodsVector.cbegin(), methodsVector.cend(), "DELETE") != std::end(methodsVector))
		DELETE = true;

	if (static_cast<int>(methodsVector.size()) != static_cast<int>(GET) + static_cast<int>(POST) + static_cast<int>(DELETE))
		throw InvalidConfigFileException();
}

std::string RouteConfig::extractIndex(std::string& locationBlock) {
	return (Parsing::extractVariable(locationBlock, "index"));
}

bool RouteConfig::extractListing(std::string& locationBlock) {
	const std::string	listing	= Parsing::extractVariable(locationBlock, "listing");

	if (listing == "on")
		return (true);
	if (listing == "off")
		return (false);
	throw InvalidConfigFileException();
}

std::string RouteConfig::extractRootDir(std::string& locationBlock) {
	return (Parsing::extractVariable(locationBlock, "root"));
}

std::string RouteConfig::extractUploadDir(std::string& locationBlock) {
	return (Parsing::extractVariable(locationBlock, "upload_dir"));
}

RouteConfig::t_redirection RouteConfig::extractRedirection(std::string& locationBlock) {
	t_redirection			        redirection = {"", 0};
	const std::string				redirection_string = Parsing::extractVariable(locationBlock, "return");
	const std::vector<std::string>	methodsVector = CppSplit::cppSplit(redirection_string, ' ');

	if (methodsVector.size() != 2)
		throw InvalidConfigFileException();

	redirection.code = StrictUtoi::strictUtoi(methodsVector.front());
	if (redirection.code < 300 || redirection.code > 399 || !httpCodes.contains(redirection.code))
		throw InvalidConfigFileException();
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

std::string RouteConfig::getIndex() const {
	return index;
}

bool RouteConfig::getListing() const {
    return (listing);
}

std::string RouteConfig::getRootDir() const {
    return (rootDir);
}

std::string RouteConfig::getUploadDir() const {
	return (uploadDir);
}

RouteConfig::t_redirection RouteConfig::getRedirection() const {
	return redirection;
}
