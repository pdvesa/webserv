//
// Created by jules on 12/09/2024.
//

#ifndef ROUTE_HPP
# define ROUTE_HPP

#include <string>
#include <vector>
#include <algorithm>

#include <CppSplit.hpp>
#include <Parsing.hpp>
#include <IsBlank.hpp>
#include <InvalidConfigFileException.hpp>
#include <HttpCodes.hpp>
#include <http_methods.h>

class RouteConfig {

	public:
		struct t_redirection {
			std::string	path;
			u_int		code;
		};

	private:
		bool			GET{};
		bool			POST{};
		bool			DELETE{};
		std::string		index;
		bool			listing{};
		std::string		rootDir;
		std::string		uploadDir;
		t_redirection	redirection;

		static void				extractMethods(bool& GET, bool& POST, bool& DELETE, std::string& locationBlock);
		static std::string		extractIndex(std::string& locationBlock);
		static bool				extractListing(std::string& locationBlock);
		static std::string		extractRootDir(std::string& locationBlock);
		static std::string		extractUploadDir(std::string& locationBlock);
		static t_redirection	extractRedirection(std::string& locationBlock);

	public:
		RouteConfig() = default;
		RouteConfig(const bool& GET,
			const bool& POST,
			const bool& DELETE,
			const std::string& index,
			const bool& listing,
			const std::string& rootDir,
			const std::string& uploadDir,
			const t_redirection& redirection);
		RouteConfig(const RouteConfig& other);
		~RouteConfig();

		RouteConfig&	operator=(const RouteConfig& other);
		bool			operator==(const RouteConfig&) const;

		static RouteConfig	fromLocationBlock(std::string& locationBlock);

		bool			isRedirection() const;
		bool			isMethodAllowed(t_method method) const;
	
		bool			getGET() const;
		bool			getPOST() const;
		bool			getDELETE() const;
		std::string		getIndex() const;
		bool			getListing() const;
		std::string		getRootDir() const;
		std::string		getUploadDir() const;
		t_redirection	getRedirection() const;
};

#endif
