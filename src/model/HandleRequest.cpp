//
// Created by jules on 14/11/2024.
//

#include <HandleRequest.hpp>

std::string HandleRequest::handleGet(const std::string& target, const std::string& location) {
	if (isDirectory(location)) {
		return (listingBody(target, location));
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
	if (access(location.c_str(), W_OK) != 0 || remove(location.c_str()) != 0)
		return ("<html><body><h1>Error</h1><p>Cannot delete file</p></body></html>");
	return ("");
}

std::string HandleRequest::handlePost(const std::string& location, const std::string& filename, std::vector<unsigned char>& content) {
	if (access(location.c_str(), W_OK) != 0)
		return ("<html><body><h1>Error</h1><p>Cannot write location</p></body></html>");
	if (filename.empty()) {
		std::stringstream	buffer;

		for (const unsigned char c : content)
			buffer << c;

		return (buffer.str());
	} else {
		if (access((location + "/" + filename).c_str(), F_OK) == 0)
			return ("<html><body><h1>Error</h1><p>Cannot write file</p></body></html>");

		std::ofstream	targetFile(location + "/" + "name");

		if (!targetFile.is_open())
			return ("<html><body><h1>Error</h1><p>Cannot write file</p></body></html>");
		for (const unsigned char c : content)
			targetFile << c;
		targetFile.close();
		return ("");
	}
}

bool	HandleRequest::isDirectory(const std::string& path) {
	struct stat statbuf;

	if (stat(path.c_str(), &statbuf) != 0)
		return (false);
	return (S_ISDIR(statbuf.st_mode));
}

std::string HandleRequest::listingBody(const std::string& target, const std::string& location) {
	std::ostringstream	content;
	DIR*				directory;
	dirent*				entry;

	if ((directory = opendir(location.c_str())) != nullptr) {
		content << "<html><body><h1>Directory listing for " << target << "</h1><ul>";
		while ((entry = readdir(directory)) != nullptr) {
			std::string name = entry->d_name;
			if (name != "." && name != "..") {
				content << "<li><a href=\"" << target << "/" << name << "\">" << name << "</a></li>";
			}
		}
		closedir(directory);
		content << "</ul></body></html>";
	} else {
		throw std::runtime_error("Could not open directory");
	}
	return (content.str());
}
