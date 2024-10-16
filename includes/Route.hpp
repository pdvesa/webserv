//
// Created by jules on 12/09/2024.
//

#ifndef ROUTE_HPP
# define ROUTE_HPP

#include <list>
#include <map>
#include <string>

class Route {
	private:
		const bool							GET;
		const bool							POST;
		const bool							DELETE;
		std::map<std::string, std::string>	locations;
		const bool							listing;
		const std::string					defaultFile;

		Route();
		Route(const bool& GET, const bool& POST, const bool& DELETE, std::map<std::string, std::string> locations,
			const bool& listing, const std::string& defaultFile);
		Route(const Route& other);

		Route& operator=(const Route& other);

	public:
		static Route	fromVariableBlock(const std::list<std::string>& routeBlock);
};

#endif
