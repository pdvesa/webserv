//
// Created by jules on 14/11/2024.
//

#include <HandleRequest.hpp>

std::string HandleRequest::handleGet(const std::string& location) {
	if (isDirectory(location)) {
		return (listingBody(location));
	}
	std::ifstream	targetFile(location);

	if (!targetFile.is_open()) {
		throw std::runtime_error("File not found");
	}
	std::stringstream buffer;
	buffer << targetFile.rdbuf();
	targetFile.close();
	return (buffer.str());
}

std::string HandleRequest::handleDelete(const std::string& location) {
	if (remove(location.c_str()) != 0)
		throw std::runtime_error("File could not be deleted");
	return ("");
}

std::string HandleRequest::handlePost(const std::string& location, std::vector<unsigned char>& content) {
	std::ofstream	targetFile(location);

	if (!targetFile.is_open())
		throw std::runtime_error("Cannot create file at location");
	for (const unsigned char c : content)
		targetFile << c;
	targetFile.close();
	return ("");
}

bool	HandleRequest::isDirectory(const std::string& path) {
	struct stat statbuf;

	if (stat(path.c_str(), &statbuf) != 0)
		return (false);
	return (S_ISDIR(statbuf.st_mode));
}

std::string HandleRequest::listingBody(const std::string& location) {
	std::ostringstream	content;
	DIR					*directory;
	struct dirent		*entry;

	if ((directory = opendir(location.c_str())) != nullptr) {
		content << "<html><body><h1>Directory listing for " << location << "</h1><ul>";
		while ((entry = readdir(directory)) != nullptr) {
			std::string name = entry->d_name;
			if (name != "." && name != "..") {
				content << "<li><a href=\"" << location << "/" << name << "\">" << name << "</a></li>";
			}
		}
		closedir(directory);
		content << "</ul></body></html>";
	} else {
		throw std::runtime_error("Could not open directory");
	}
	return (content.str());
}
