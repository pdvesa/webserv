//
// Created by jules on 16/10/2024.
//

#ifndef WEBSERVCONTROLLER_HPP
#define WEBSERVCONTROLLER_HPP

#include <vector>

#include <ServerConfig.hpp>

class WebservController {
	private:
		std::vector<ServerConfig>	serverConfigs;

		WebservController();
		WebservController(const WebservController&);

		WebservController& operator=(const WebservController&);

	public:
		~WebservController();

		WebservController(const std::string& configFilePath);

		void	run();
};



#endif //WEBSERVCONTROLLER_HPP
