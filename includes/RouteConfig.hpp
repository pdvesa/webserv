//
// Created by jules on 12/09/2024.
//

#ifndef ROUTE_HPP
# define ROUTE_HPP

#include <list>
#include <map>
#include <string>
#include <vector>
#include <algorithm>

#include <CppSplit.hpp>
#include <Parsing.hpp>
#include <ServerConfig.hpp>

class ServerConfig;

class RouteConfig {

	public:
		struct t_redirection {
			std::string	path;
			u_int		code;
		};

	private:
		const bool			GET;
		const bool			POST;
		const bool			DELETE;
		const std::string	index;
		const bool			listing;
		const std::string	rootDir;
		const t_redirection	redirection;

		RouteConfig();

		RouteConfig& operator=(const RouteConfig& other);

		static void				extractMethods(bool& GET, bool& POST, bool& DELETE, std::string& locationBlock);
		static std::string		extractIndex(std::string& locationBlock);
		static bool				extractListing(std::string& locationBlock);
		static std::string		extractRootDir(std::string& locationBlock);
		static t_redirection	extractRedirection(std::string& locationBlock);

	public:
		RouteConfig(const bool& GET, const bool& POST, const bool& DELETE, const std::string& index,
			const bool& listing, const std::string& rootDir, const t_redirection& redirection);
		RouteConfig(const RouteConfig& other);
		~RouteConfig();

		bool	operator==(const RouteConfig&) const;

		static RouteConfig	fromLocationBlock(std::string& locationBlock);

		bool		getGET() const;
		bool		getPOST() const;
		bool		getDELETE() const;
		std::string getIndex() const;
		bool		getListing() const;
		std::string	getRootDir() const;
		t_redirection getRedirection() const;
};

#endif
