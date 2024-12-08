//
// Created by jules on 14/11/2024.
//

#include <HandleRequest.hpp>

std::string HandleRequest::handleGet(const std::string& targetUrl, const std::string& serverLocation, bool listing, std::string& contentType) {
	if (isDirectory(serverLocation)) {
		if (listing) {
			contentType = "text/html";
			return (listingBody(targetUrl, serverLocation));
		}
		throw std::runtime_error("401");
	}
	std::ifstream	targetFile(serverLocation);

	if (!targetFile.is_open()) {
		throw std::runtime_error("404");
	}
	contentType = getContentType(serverLocation);
	std::stringstream buffer;
	buffer << targetFile.rdbuf();
	targetFile.close();
	return (buffer.str());
}

std::string HandleRequest::handleDelete(const std::string& fileToDelete) {
	if (access(fileToDelete.c_str(), F_OK) != 0)
		throw std::runtime_error("401");
	if (access(fileToDelete.c_str(), W_OK) != 0)
		throw std::runtime_error("404");
	if (remove(fileToDelete.c_str()) != 0)
		throw std::runtime_error("500");
	return ("<html><body><h1>Success</h1></body></html>");
}

std::string HandleRequest::handlePost(const std::string& uploadLocation, const std::string& contentType, std::vector<unsigned char>& content) {
	std::stringstream	buffer;

	for (const unsigned char c : content) {
		buffer << c;
	}

	size_t boundaryPos = contentType.find("boundary=");
	if (boundaryPos == std::string::npos) {
		return (buffer.str());
	} else {
		if (access(uploadLocation.c_str(), F_OK) != 0) {
			if (mkdir(uploadLocation.c_str(), 0755) != 0) {
				throw std::runtime_error("500");
			}
		} else if (access(uploadLocation.c_str(), W_OK) != 0) {
			throw std::runtime_error("401");
		}
		
		std::string boundaryString = contentType.substr(boundaryPos + 9, contentType.size() - boundaryPos - 9);
		std::vector<std::string>	linesContent = CppSplit::cppSplit(buffer.str(), '\n');

		if (linesContent.size() < 5)
			throw std::runtime_error("400");

		if (linesContent[1].compare(2, boundaryString.size(), boundaryString) != 0)
			throw std::runtime_error("400");
		if (linesContent[linesContent.size() - 1].compare(2, boundaryString.size(), boundaryString) != 0)
			throw std::runtime_error("400");


		try {
			std::string	filename = Parsing::extractVariable(linesContent[2],"filename=");

			if (access((uploadLocation + "/" + filename).c_str(), F_OK) == 0)
				throw std::runtime_error("401");

			std::ofstream	targetFile(uploadLocation + "/" + filename);

			if (!targetFile.is_open())
				throw std::runtime_error("500");

			for (unsigned long i = 4; i < linesContent.size() - 1; i++) {
				targetFile << linesContent[i];
			}
			targetFile.close();
			return ("<html><body><h1>Success</h1></body></html>");
		} catch (Parsing::VariableNotFoundException&) {
			throw std::runtime_error("400");
		}
	}
}

std::string HandleRequest::handleCGI(HttpRequest request) {
	CGI cgi(request);
	if (cgi.getCGIStatus())	
		throw std::runtime_error("500");
	return (cgi.getCGIResponse());
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
		throw std::runtime_error("500");
	}
	return (content.str());
}

std::string HandleRequest::getContentType(const std::string& filePath) {
	static const std::map<std::string, std::string> extensionToMimeType = {
		{".html", "text/html"},
		{".css", "text/css"},
		{".js", "application/javascript"},
		{".json", "application/json"},
		{".png", "image/png"},
		{".jpg", "image/jpeg"},
		{".jpeg", "image/jpeg"},
		{".gif", "image/gif"},
		{".txt", "text/plain"},
		{".xml", "application/xml"},
		{".svg", "image/svg+xml"},
		{".ico", "image/vnd.microsoft.icon"},
		{".ttf", "font/ttf"},
		{".woff", "font/woff"},
		{".woff2", "font/woff2"},
		{".mp3", "audio/mpeg"},
		{".mp4", "video/mp4"},
		{".pdf", "application/pdf"},
		{".zip", "application/zip"},
		{".rar", "application/vnd.rar"},
		{".csv", "text/csv"},
		{".bmp", "image/bmp"},
		{".webp", "image/webp"},
		{".ogg", "audio/ogg"},
		{".wav", "audio/wav"},
		{".avi", "video/x-msvideo"},
		{".mov", "video/quicktime"},
		{".mpeg", "video/mpeg"},
		{".flv", "video/x-flv"},
		{".wasm", "application/wasm"}
	};

	size_t extensionIndex = filePath.find_last_of('.');
	if (extensionIndex != std::string::npos) {
		std::string extension = filePath.substr(extensionIndex);

		if (extensionToMimeType.contains(extension))
			return (extensionToMimeType.at(extension));
	}
	return ("application/octet-stream");
}
