//
// Created by jules on 12/09/2024.
//

#include <ServerConfig.hpp>

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
				const std::vector<Route>& routes) :
	host(host),
	port(port),
	names(names),
	errorsPages(errorsPages),
	maxClientBodySize(maxClientBodySize),
	routes(routes) { }

ServerConfig ServerConfig::parseServer(std::string& content) {
	std::string serverBlock;
	try {
		serverBlock = Parsing::extractBlock(content, "server");
	} catch (Parsing::BlockNotFoundException&) {
		throw InvalidConfigFileException();
	}
	std::string						parsedHost = parseServerHostName(serverBlock);
	u_int							parsedPort = parseServerPort(serverBlock);
	std::vector<std::string>		parsedServerName = parseServerNames(serverBlock);
	std::map<u_int, std::string>	parsedErrorPages = parseServerErrorsPages(serverBlock);
	u_int							parsedBodySize = parseServerMaxClientBodySize(serverBlock);
	std::vector<Route>				parsedRoutes = parseServerRoutes(serverBlock);

	return (ServerConfig(parsedHost, parsedPort, parsedServerName, parsedErrorPages, parsedBodySize, parsedRoutes));
}

std::string ServerConfig::parseServerHostName(std::string& serverBlock) {
	return (Parsing::extractVariable(serverBlock, "hostname"));
}

u_int ServerConfig::parseServerPort(std::string& serverBlock) {
	return (Parsing::extractInteger(serverBlock, "port"));
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
					u_int	errorCode = Parsing::extractInteger(errorPage[i], "error_code");
					std::string errorPath = errorPage.back();
					parsedErrorPages.insert({errorCode, errorPath});
				}
			} else
				throw std::runtime_error("");
		}
	} catch (Parsing::VariableNotFoundException&) {}
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

std::vector<Route>	ServerConfig::parseServerRoutes(std::string& serverBlock) {
	std::vector<Route> parsedRoutes;
	try {
		while (true) {
			std::list<std::string>	routeData = Parsing::extractVariableBlock(serverBlock, "location");
			Route					route = Route::fromVariableBlock(routeData);

			parsedRoutes.push_back(route);
		}
	} catch (Parsing::VariableNotFoundException&) { }
	return (parsedRoutes);
}
