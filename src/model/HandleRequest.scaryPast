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
			if (!std::filesystem::create_directories(uploadLocation)) {
				throw std::runtime_error("500");
			}
		} else if (access(uploadLocation.c_str(), W_OK) != 0) {
			throw std::runtime_error("401");
		}
		std::string boundaryString = contentType.substr(boundaryPos + 9, contentType.size() - boundaryPos - 9);

		std::string	body = buffer.str();

		size_t	firstLineEnd = body.find('\n');
		if (firstLineEnd == std::string::npos)
			throw std::runtime_error("400");
		size_t	firstBoundaryPos = body.substr(0, firstLineEnd).find(boundaryString);
		if (firstBoundaryPos == std::string::npos)
			throw std::runtime_error("400");

		if (body.at(body.size() - 1) == '\n')
			body = body.substr(0, body.size() - 1);
		size_t	lastNewLine = body.find_last_of('\n');
		if (lastNewLine == std::string::npos || lastNewLine == firstLineEnd || lastNewLine <= firstLineEnd + 2)
			throw std::runtime_error("400");
		size_t	endBoundaryPos = body.substr(lastNewLine + 1).find(boundaryString);
		if (endBoundaryPos == std::string::npos) {
			throw std::runtime_error("400");
		}

		body = body.substr(body.find('\n') + 1, lastNewLine - (firstLineEnd + 2));

		size_t filenameLineEnd = body.find('\n');
		if (filenameLineEnd == std::string::npos)
			throw std::runtime_error("400");

		std::string filenameLine = body.substr(0, filenameLineEnd);
		body = body.substr(filenameLineEnd + 1, body.size());

		std::string	filename;
		try {
			filename = Parsing::extractVariable(filenameLine,"filename=");
		} catch (Parsing::VariableNotFoundException&) {
			throw std::runtime_error("400");
		}
		if (access((uploadLocation + "/" + filename).c_str(), F_OK) == 0)
			throw std::runtime_error("401");
		std::string	filepath = uploadLocation + "/" + filename;
		int	fd = open(filepath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
		if (fd == -1)
			throw std::runtime_error("500");

		for (u_int i = 0; i < 2; i++) {
			size_t endLinePos = body.find('\n');
			if (endLinePos == std::string::npos) {
				close(fd);
				throw std::runtime_error("400");
			}
			body = body.substr(endLinePos + 1, body.size());
		}

		ssize_t result = write(fd , body.c_str(), body.size());
		close(fd);
		if (result == -1)
			throw std::runtime_error("500");
		else if (result == 0 && !body.empty())
			throw std::runtime_error("500");

		return ("<html><body><h1>Success</h1></body></html>");
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
