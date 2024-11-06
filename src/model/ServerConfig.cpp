//
// Created by jules on 12/09/2024.
//

#include <ServerConfig.hpp>

bool ServerConfig::operator==(const ServerConfig& other) const {
	return (host == other.host &&
				port == other.port &&
				names == other.names &&
				errorsPages == other.errorsPages &&
				maxClientBodySize == other.maxClientBodySize &&
				routes == other.routes);
}

std::vector<ServerConfig> ServerConfig::fromConfigFile(const std::string& filePath) {
	std::ifstream	configFile(filePath);

	if (!configFile.is_open())
		throw std::runtime_error("Could not open config file " + filePath);

	std::stringstream	buffer;
	buffer << configFile.rdbuf();
	std::string	content = buffer.str();

	std::vector<ServerConfig>	servers;

	while (!IsBlank::isBlank(content)) {
		ServerConfig	newServer = parseServer(content);

		servers.push_back(newServer);
	}
	return (servers);
}

ServerConfig::ServerConfig(const std::string& host,
							const u_int& port,
							const std::vector<std::string>& names,
							const std::map<u_int, std::string>& errorsPages,
							const u_int& maxClientBodySize,
							const std::vector<RouteConfig>& routes) :
	host(host),
	port(port),
	names(names),
	errorsPages(errorsPages),
	maxClientBodySize(maxClientBodySize),
	routes(routes) { }

ServerConfig::ServerConfig(const ServerConfig& other) : host(other.host),
	port(other.port),
	names(other.names),
	errorsPages(other.errorsPages),
	maxClientBodySize(other.getMaxClientBodySize()),
	routes(other.routes) { }

ServerConfig ServerConfig::parseServer(std::string& content) {
	std::string serverBlock;
	try {
		serverBlock = Parsing::extractBlock(content, "server");
	} catch (Parsing::BlockNotFoundException&) {
		throw InvalidConfigFileException();
	}
	const std::string					parsedHost = parseServerHostName(serverBlock); //Mandatory
	const u_int							parsedPort = parseServerPort(serverBlock); //Mandatory
	const std::vector<std::string>		parsedServerName = parseServerNames(serverBlock); //Optional
	const std::map<u_int, std::string>	parsedErrorPages = parseServerErrorsPages(serverBlock); //Mandatory
	const u_int							parsedBodySize = parseServerMaxClientBodySize(serverBlock); //Optional
	const std::vector<RouteConfig>		parsedRoutes = parseServerRoutes(serverBlock); //Mandatory

	return (ServerConfig(parsedHost, parsedPort, parsedServerName, parsedErrorPages, parsedBodySize, parsedRoutes));
}

std::string ServerConfig::parseServerHostName(std::string& serverBlock) {
	return (Parsing::extractVariable(serverBlock, "hostname"));
}

u_int ServerConfig::parseServerPort(std::string& serverBlock) {
	const u_int port = Parsing::extractInteger(serverBlock, "port");

	if (port == 0 || port > 65535)
		throw InvalidConfigFileException();
	return (port);
}

std::vector<std::string> ServerConfig::parseServerNames(std::string& serverBlock) {
	std::vector<std::string> parsedServerName;
	try {
		parsedServerName = CppSplit::cppSplit(
			Parsing::extractVariable(serverBlock, "server_name"), ' ');
	} catch (Parsing::VariableNotFoundException&) { }
	return (parsedServerName);
}

std::map<u_int, std::string> ServerConfig::parseServerErrorsPages(std::string& serverBlock) {
	std::map<u_int, std::string> parsedErrorPages;
	try {
		while (true) {
			std::vector<std::string>	errorPage = CppSplit::cppSplit(
			Parsing::extractVariable(serverBlock, "error_page"), ' ');
			if (errorPage.size() > 1) {
				for (u_int i = 0; i < errorPage.size() - 1; i++) {
					u_int	errorCode = StrictUtoi::strictUtoi(errorPage[i]);
					std::string errorPath = errorPage.back();
					parsedErrorPages.insert({errorCode, errorPath});
				}
			} else
				throw std::runtime_error("");
		}
	} catch (Parsing::VariableNotFoundException&) {}
	if (parsedErrorPages.empty())
		throw InvalidConfigFileException("No error pages");
	return (parsedErrorPages);
}

u_int ServerConfig::parseServerMaxClientBodySize(std::string& serverBlock) {
	u_int	parsedBodySize;
	try {
		parsedBodySize = Parsing::extractInteger(serverBlock, "client_max_body_size");
		if (parsedBodySize > CLIENT_MAX_BODY_SIZE_LIMIT)
			parsedBodySize = CLIENT_MAX_BODY_SIZE_LIMIT;
	} catch (Parsing::VariableNotFoundException&) {
		parsedBodySize = CLIENT_MAX_BODY_SIZE_LIMIT;
	}
	return (parsedBodySize);
}

std::vector<RouteConfig>	ServerConfig::parseServerRoutes(std::string& serverBlock) {
	std::vector<RouteConfig> parsedRoutes;
	try {
		while (true) {
			std::list<std::string>	routeData = Parsing::extractVariableBlock(serverBlock, "location");
			RouteConfig					route = RouteConfig::fromVariableBlock(routeData);

			parsedRoutes.push_back(route);
		}
	} catch (Parsing::BlockNotFoundException&) { }
	if (parsedRoutes.empty())
		throw InvalidConfigFileException("No route found");
	return (parsedRoutes);
}

std::string ServerConfig::getHost() const {
    return (host);
}

u_int ServerConfig::getPort() const {
    return (port);
}

std::vector<std::string> ServerConfig::getNames() const {
    return (names);
}

std::map<u_int, std::string> ServerConfig::getErrorsPages() const {
    return (errorsPages);
}

u_int ServerConfig::getMaxClientBodySize() const {
    return (maxClientBodySize);
}

std::vector<RouteConfig> ServerConfig::getRoutes() const {
    return (routes);
}

ServerConfig::InvalidConfigFileException::InvalidConfigFileException() { }

ServerConfig::InvalidConfigFileException::InvalidConfigFileException(const std::string& msg): message(msg) { }

ServerConfig::InvalidConfigFileException::~InvalidConfigFileException() { }

const char* ServerConfig::InvalidConfigFileException::what() const noexcept {
	if (message.empty()) {
		return "Invalid config file";
	} else {
		return message.c_str();
	}
}

ServerConfig::~ServerConfig() { }
