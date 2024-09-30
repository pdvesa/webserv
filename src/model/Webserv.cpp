//
// Created by jules on 12/09/2024.
//

#include <Webserv.hpp>

Webserv Webserv::fromConfigFile(const std::string& filePath) {
	std::ifstream	configFile(filePath);

	if (!configFile.is_open())
		throw std::runtime_error("Could not open config file " + filePath);

	std::stringstream	buffer;
	buffer << configFile.rdbuf();
	std::string	content = buffer.str();
	std::string	serverBlock = Parsing::extractBlock(content, "server");

}
