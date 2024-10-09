//
// Created by jules on 12/09/2024.
//

#include <CppSplit.hpp>
#include <iostream>
#include <Webserv.hpp>

Webserv Webserv::fromConfigFile(const std::string& filePath) {
	std::ifstream	configFile(filePath);

	if (!configFile.is_open())
		throw std::runtime_error("Could not open config file " + filePath);

	std::stringstream	buffer;
	buffer << configFile.rdbuf();
	std::string	content = buffer.str();
	while (!content.empty()) {
		std::string serverBlock = Parsing::extractBlock(content, "server");

		u_int	port = Parsing::extractInteger(serverBlock, "port");

		std::vector<std::string> serverName;
		try {
			serverName = CppSplit::cppSplit(
			Parsing::extractVariable(serverBlock, "server_name"), ' ');
		} catch (...) {
			serverName = std::vector<std::string>();
		}

		u_int	bodySize;
		try {
			bodySize = Parsing::extractInteger(serverBlock, "client_max_body_size");
			if (bodySize > CLIENT_MAX_BODY_SIZE_LIMIT)
				throw std::runtime_error("Server body size is too big");
		} catch (...) {
			bodySize = CLIENT_MAX_BODY_SIZE_LIMIT;
		}

		std::map<u_int, std::string> errorPages;
		while (true) {
			try {
				std::vector<std::string>	errorPage = CppSplit::cppSplit(
				Parsing::extractVariable(serverBlock, "error_page"), ' ');
				if (errorPage.size() > 1) {
					for (u_int i = 0; i < errorPage.size() - 1; i++) {
						try {
							u_int	errorCode = Parsing::extractInteger(errorPage[i], "error_code");
							std::string errorPath = errorPage.back();
							errorPages.insert({errorCode, errorPath});
						} catch (...) {

						}

					}
				} else
					throw std::runtime_error("");
			} catch (...) {
				break ;
			}
		}
	}
}
