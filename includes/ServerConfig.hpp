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
#include <RouteConfig.hpp>
#include <Parsing.hpp>

#ifndef CLIENT_MAX_BODY_SIZE_LIMIT
# define CLIENT_MAX_BODY_SIZE_LIMIT 8192
#endif

class RouteConfig;

class ServerConfig {
	private:
		std::string							host;
		u_int								port;
		std::vector<std::string>			names;
		std::map<u_int, std::string>		errorsPages;
		u_int								maxClientBodySize;
		std::map<std::string ,RouteConfig>	routes;

	public:
		static std::vector<ServerConfig>	fromConfigFile(const std::string&);


		ServerConfig(const std::string& host, const u_int& port, const std::vector<std::string>& names,
				const std::map<u_int, std::string>& errorsPages, const u_int& maxClientBodySize,
				const std::map<std::string, RouteConfig>& routes);
		ServerConfig(const ServerConfig&);
		~ServerConfig();

		ServerConfig&	operator=(const ServerConfig&);
		bool			operator==(const ServerConfig&) const;

		std::string							getHost() const;
    	u_int								getPort() const;
    	std::vector<std::string>			getNames() const;
    	std::map<u_int, std::string>		getErrorsPages() const;
    	u_int								getMaxClientBodySize() const;
    	std::map<std::string, RouteConfig>	getRoutes() const;

		class InvalidConfigFileException : public std::exception {
			std::string	message;

			public:
				InvalidConfigFileException();
				explicit InvalidConfigFileException(const std::string& msg);
				~InvalidConfigFileException() override;

				const char* what() const noexcept override;

			private:
				InvalidConfigFileException(const InvalidConfigFileException& other);

				InvalidConfigFileException& operator=(const InvalidConfigFileException& other);
		};

	private:
		ServerConfig();


		static ServerConfig							parseServer(std::string& content);
		static std::string							parseServerHostName(std::string& serverBlock);
		static u_int								parseServerPort(std::string& serverBlock);
		static std::vector<std::string>				parseServerNames(std::string& serverBlock);
		static std::map<u_int, std::string>			parseServerErrorsPages(std::string& serverBlock);
		static u_int								parseServerMaxClientBodySize(std::string& serverBlock);
		static std::map<std::string, RouteConfig>	parseServerRoutes(std::string& serverBlock);
};

#endif
