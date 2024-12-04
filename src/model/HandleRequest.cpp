//
// Created by jules on 14/11/2024.
//

#include <HandleRequest.hpp>

std::string HandleRequest::handleGet(const std::string& targetUrl, const std::string& serverLocation, bool listing) {
	if (isDirectory(serverLocation)) {
		if (listing)
			return (listingBody(targetUrl, serverLocation));
		throw std::runtime_error("File not found");
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
		throw std::runtime_error("File not found");
	return ("");
}

std::string HandleRequest::handlePost(const std::string& uploadLocation, const std::string& contentType, std::vector<unsigned char>& content) {
	(void) contentType;
	if (access(uploadLocation.c_str(), W_OK) != 0)
		throw std::runtime_error("Cannot write location");

	std::stringstream	buffer;

	for (const unsigned char c : content)
		buffer << c;

	std::vector<std::string>	linesContent = CppSplit::cppSplit(buffer.str(), '\n');
	if (linesContent.size() < 5)
		throw std::runtime_error("Filename not found");
	try {
		std::string	filename = Parsing::extractVariable(linesContent[1],"filename=");

		if (access((uploadLocation + "/" + filename).c_str(), F_OK) == 0)
			throw std::runtime_error("File already exists");

		std::ofstream	targetFile(uploadLocation + "/" + "name");

		if (!targetFile.is_open())
			throw std::runtime_error("Error creating file");

		for (unsigned long i = 4; i < linesContent.size() -1; i++) {
			targetFile << linesContent[i];
		}
		targetFile.close();
		return ("<html><body><h1>Success</h1></body></html>");
	} catch (Parsing::VariableNotFoundException&) {
		throw std::runtime_error("Filename not found");
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

	std::cerr << targetUrl << std::endl;
	std::cerr << serverLocation << std::endl;
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
