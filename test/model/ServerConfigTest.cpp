//
// Created by jcayot on 10/25/24.
//

#include <fstream>
#include <gtest/gtest.h>
#include <ServerConfig.hpp>

TEST(ServerConfigParseServerTestSuite, SimpleBlock) {
	std::string serverBlock = "server{ hostname www.alpaca.com\n"
						   "port 1\n\n"
						   "server_name alpaca lama\n"
						   "error_page 404 405 /error/general.html\n\n"
						   "client_max_body_size 2\n\n"
						   "location /saku/ {\n"
						   "    methods GET POST\n"
						   "    listing on\n"
						   "    root /svesa/\n"
						   "}}\n";
	ServerConfig config = ServerConfig::parseServer(serverBlock);
	ASSERT_STREQ(config.getHost().c_str(), "www.alpaca.com");
	ASSERT_EQ(config.getNames().size(), 2);
	ASSERT_STREQ(config.getNames()[0].c_str(), "alpaca");
	ASSERT_STREQ(config.getNames()[1].c_str(), "lama");
	ASSERT_EQ(config.getPort(), 1);
	ASSERT_EQ(config.getErrorsPages().size(), 2);
	ASSERT_STREQ(config.getErrorsPages().at(404).c_str(), "/error/general.html");
	ASSERT_STREQ(config.getErrorsPages().at(405).c_str(), "/error/general.html");
	ASSERT_EQ(config.getMaxClientBodySize(), 2);
	ASSERT_EQ(config.getRoutes().size(), 1);
}