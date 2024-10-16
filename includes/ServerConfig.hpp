//
// Created by jules on 12/09/2024.
//

#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

#include <string>
#include <fstream>
#include <list>
#include <sstream>
#include <iostream>

#include <IsBlank.hpp>
#include <CppSplit.hpp>
#include <Route.hpp>
#include <Parsing.hpp>

#ifndef CLIENT_MAX_BODY_SIZE_LIMIT
# define CLIENT_MAX_BODY_SIZE_LIMIT 8192
#endif

class ServerConfig {
	private:
		const std::string					host;
		const u_int							port;
		const std::vector<std::string>		names;
		const std::map<u_int, std::string>	errorsPages;
		const u_int							maxClientBodySize;
		const std::vector<Route>			routes;

		ServerConfig();
		ServerConfig(const std::string& host, const u_int& port, const std::vector<std::string>& names,
				const std::map<u_int, std::string>& errorsPages, const u_int& maxClientBodySize,
				const std::vector<Route>& routes);
		ServerConfig(const ServerConfig& other);

		ServerConfig& operator=(const ServerConfig& other);

		static ServerConfig						parseServer(std::string& content);
		static std::string					parseServerHostName(std::string& serverBlock);
		static u_int						parseServerPort(std::string& serverBlock);
		static std::vector<std::string>		parseServerNames(std::string& serverBlock);
		static std::map<u_int, std::string>	parseServerErrorsPages(std::string& serverBlock);
		static u_int						parseServerMaxClientBodySize(std::string& serverBlock);
		static std::vector<Route>			parseServerRoutes(std::string& serverBlock);

	public:
		static std::vector<ServerConfig>	fromConfigFile(const std::string&);

	class InvalidConfigFileException : public std::exception {
		std::string message;

		public:
		const char* what() const noexcept override {
			if (message.empty()) {
				return "Invalid config file";
			} else {
				return message.c_str();
			}
		}

		InvalidConfigFileException() = default;

		explicit InvalidConfigFileException(const std::string& msg) : message(msg) {}

		~InvalidConfigFileException() override = default;
	};
};



#endif
