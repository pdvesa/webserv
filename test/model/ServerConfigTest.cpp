//
// Created by jcayot on 10/25/24.
//

#include <fstream>
#include <gtest/gtest.h>
#include <ServerConfig.hpp>

const std::string CONFIG_FILE_NAME = "configfile.example";
const std::string VALID_CONFIG_FILE_CONTENT =
"server { hostname www.alpaca.com\n"
"port 98\n\n"
"server_name alpaca lama\n"
"error_page 404 405 /error/general.html\n\n"
"client_max_body_size 2\n\n"
"location /saku/ {\n"
"	methods GET POST\n"
"	listing on\n"
"	index index.html\n"
"	root /svesa/\n"
"}}\n"

"server { hostname www.lama.com\n"
"port 56\n\n"
"server_name alpaca lama\n"
"error_page 405 /error/general.html\n\n"
"client_max_body_size 8\n\n"
"location /saku/ {\n"
"	methods GET DELETE\n"
"	listing on\n"
"	index index.html\n"
"	root /svesa/\n"
"}}\n"

"server { hostname www.peru.com\n"
"port 8080\n\n"
"server_name lama\n"
"error_page 405 /error/general.html\n\n"
"client_max_body_size 59\n\n"
"location /saku/ {\n"
"	methods GET POST\n"
"	index index.html\n"
"	listing on\n"
"	root /svesa/\n"
"}}\n";

const std::string INVALID_PORT_CONTENT =
"server { hostname www.alpaca.com\n"
"port 0\n\n"
"server_name alpaca lama\n"
"error_page 404 405 /error/general.html\n\n"
"client_max_body_size 2\n\n"
"location /saku/ {\n"
"    methods GET POST\n"
"    listing on\n"
"    root /svesa/\n"
"}}\n";

const std::string NO_SERVER_BLOCK =
"hostname www.alpaca.com\n"
"port 0\n\n"
"server_name alpaca lama\n"
"error_page 404 405 /error/general.html\n\n"
"client_max_body_size 2\n\n"
"location /saku/ {\n"
"    methods GET POST\n"
"    listing on\n"
"    root /svesa/\n"
"}\n";

void createConfigFile(const std::string& filePath, const std::string& content) {
	std::ofstream file(filePath);
	if (!file.is_open())
		FAIL() << "Failed to open config file code might be right";
	file << content;
	file.close();
}

void removeConfigFile(const std::string& filePath) {
	std::remove(filePath.c_str());
}

TEST(ServerConfigFromConfigFileTestSuite, SimpleConfigFile) {
	createConfigFile(CONFIG_FILE_NAME, VALID_CONFIG_FILE_CONTENT);
	std::vector<ServerConfig> serverConfigs = ServerConfig::fromConfigFile(CONFIG_FILE_NAME);
	ASSERT_EQ(serverConfigs.size(), 3);
	ServerConfig excepted = ServerConfig(
		"www.alpaca.com",
		98,
		std::vector<std::string>{"alpaca", "lama"},
		std::map<u_int, std::string>{{404, "/error/general.html"}, {405, "/error/general.html"}},
		2,
		std::vector{RouteConfig(true, true, false, "/saku/", "index.html",true, "/svesa/", {"", 0})});
	ASSERT_EQ(serverConfigs[0], excepted);
	ServerConfig excepted1 = ServerConfig(
		"www.lama.com",
		56,
		std::vector<std::string>{"alpaca", "lama"},
		std::map<u_int, std::string>{ {405, "/error/general.html"}},
		8,
		std::vector{RouteConfig(true, false, true, "/saku/", "index.html", true, "/svesa/", {"", 0})});
	ASSERT_EQ(serverConfigs[1], excepted1);
	ServerConfig excepted2 = ServerConfig(
		"www.peru.com",
		8080,
		std::vector<std::string>{"lama"},
		std::map<u_int, std::string>{ {405, "/error/general.html"}},
		59,
		std::vector{RouteConfig(true, true, false, "/saku/", "index.html", true, "/svesa/", {"", 0})});
	ASSERT_EQ(serverConfigs[2], excepted2);
	removeConfigFile(CONFIG_FILE_NAME);
}

TEST(ServerConfigFromConfigFileTestSuite, InvalidPort) {
	createConfigFile(CONFIG_FILE_NAME, INVALID_PORT_CONTENT);
	ASSERT_THROW(ServerConfig::fromConfigFile(CONFIG_FILE_NAME), ServerConfig::InvalidConfigFileException);
	removeConfigFile(CONFIG_FILE_NAME);
}

TEST(ServerConfigFromConfigFileTestSuite, NoServerBlock) {
	createConfigFile(CONFIG_FILE_NAME, NO_SERVER_BLOCK);
	ASSERT_THROW(ServerConfig::fromConfigFile(CONFIG_FILE_NAME), ServerConfig::InvalidConfigFileException);
	removeConfigFile(CONFIG_FILE_NAME);
}