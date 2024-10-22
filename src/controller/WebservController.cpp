//
// Created by jules on 16/10/2024.
//

#include <WebservController.hpp>


WebservController::WebservController(const std::string& configFilePath) {
	serverConfigs = ServerConfig::fromConfigFile(configFilePath);
}

void	WebservController::run() {
	
}
