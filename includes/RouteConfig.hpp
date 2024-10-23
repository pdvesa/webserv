//
// Created by jules on 12/09/2024.
//

#ifndef ROUTE_HPP
# define ROUTE_HPP

#include <list>
#include <map>
#include <string>
#include <vector>

#include <CppSplit.hpp>
#include <Parsing.hpp>
#include <ServerConfig.hpp>

class RouteConfig {
	private:
		const bool							GET;
		const bool							POST;
		const bool							DELETE;
		std::string							location;
		const bool							listing;
		const std::string					rootDir;

		RouteConfig();
		RouteConfig(const bool& GET, const bool& POST, const bool& DELETE, const std::string & location,
			const bool& listing, const std::string & rootDir);
		RouteConfig(const RouteConfig& other);

		RouteConfig& operator=(const RouteConfig& other);

		static void			getMethods(bool& GET, bool& POST, bool& DELETE, std::string& locationBlock);
		static std::string	getRootDir(std::string& locationBlock);
		static bool			getListing(std::string& locationBlock);

	public:
		static RouteConfig	fromVariableBlock(std::list<std::string>& routeBlock);
};

#endif
