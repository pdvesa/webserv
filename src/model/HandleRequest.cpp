//
// Created by jules on 14/11/2024.
//

#include <HandleRequest.hpp>

std::string HandleRequest::handleGet(const std::string& targetUrl, const std::string& serverLocation, bool listing) {
	if (isDirectory(serverLocation)) {
		return (listingBody(targetUrl, serverLocation));
	}
	std::ifstream	targetFile(serverLocation);

	if (!targetFile.is_open()) {
		throw std::runtime_error("File not found");
	}
	std::stringstream buffer;
	buffer << targetFile.rdbuf();
	targetFile.close();
	return (buffer.str());
}

std::string HandleRequest::handleDelete(const std::string& fileToDelete) {
	if (access(fileToDelete.c_str(), W_OK) != 0 || remove(fileToDelete.c_str()) != 0)
		return ("<html><body><h1>Error</h1><p>Cannot delete file</p></body></html>");
	return ("");
}

std::string HandleRequest::handlePost(const std::string& uploadLocation, const std::string& filename, std::vector<unsigned char>& content) {
	if (access(uploadLocation.c_str(), W_OK) != 0)
		return ("<html><body><h1>Error</h1><p>Cannot write location</p></body></html>");
	if (filename.empty()) {
		std::stringstream	buffer;

		for (const unsigned char c : content)
			buffer << c;

		return (buffer.str());
	} else {
		if (access((uploadLocation + "/" + filename).c_str(), F_OK) == 0)
			return ("<html><body><h1>Error</h1><p>Cannot write file</p></body></html>");

		std::ofstream	targetFile(uploadLocation + "/" + "name");

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

std::string HandleRequest::listingBody(const std::string& targetUrl, const std::string& serverLocation) {
	std::ostringstream	content;
	DIR*				directory;
	dirent*				entry;

	if ((directory = opendir(serverLocation.c_str())) != nullptr) {
		content << "<html><body><h1>Directory listing for " << targetUrl << "</h1><ul>";
		while ((entry = readdir(directory)) != nullptr) {
			std::string name = entry->d_name;
			if (name != "." && name != "..") {
				content << "<li><a href=\"" << targetUrl << "/" << name << "\">" << name << "</a></li>";
			}
		}
		closedir(directory);
		content << "</ul></body></html>";
	} else {
		throw std::runtime_error("Could not open directory");
	}
	return (content.str());
}
