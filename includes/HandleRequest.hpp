//
// Created by jules on 14/11/2024.
//

#ifndef HANDLEREQUEST_HPP
#define HANDLEREQUEST_HPP

#include <BodyChunk.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <Parsing.hpp>
#include <CppSplit.hpp>

class HandleRequest {
	private:
		HandleRequest();
		HandleRequest(const HandleRequest &);
		~HandleRequest();

		HandleRequest &operator=(const HandleRequest &);

		static bool			isDirectory(const std::string& path);
		static std::string	listingBody(const std::string& targetUrl, const std::string& serverLocation);

	public:
		static std::string	handleGet(const std::string& targetUrl, const std::string& serverLocation, bool listing);
		static std::string	handleDelete(const std::string& fileToDelete);
		static std::string	handlePost(const std::string& uploadLocation, const std::string& contentType, std::vector<unsigned char>& content);
};



#endif //HANDLEREQUEST_HPP
