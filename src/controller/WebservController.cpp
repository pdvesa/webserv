//
// Created by jules on 16/10/2024.
//

#include <WebservController.hpp>


WebservController::WebservController(const std::string& configFilePath) {
	try {
		serverConfigs = ServerConfig::fromConfigFile(configFilePath);
	} catch (...) {
		exit(1);
	}
}

void	WebservController::run() {

}
