//
// Created by jules on 14/11/2024.
//

#include <HandleRequest.hpp>

std::string HandleRequest::handleGet(const std::string& location) {
	std::ifstream	targetFile(location);

	if (!targetFile.is_open())
		throw std::runtime_error("File not found");
	std::stringstream buffer;
	buffer << targetFile.rdbuf();
	targetFile.close();
	return (buffer.str());
}

void HandleRequest::handleDelete(const std::string& location) {
	if (remove(location.c_str()) != 0)
		throw std::runtime_error("File could not be deleted");
}

void HandleRequest::handlePost(const std::string& location, const std::string& content) {
	std::ofstream	targetFile(location);

	if (!targetFile.is_open())
		throw std::runtime_error("Cannot create file at location");
	targetFile << content;
	targetFile.close();
}
