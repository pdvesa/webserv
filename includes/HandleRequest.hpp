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

class HandleRequest {
	private:
		HandleRequest();
		HandleRequest(const HandleRequest &);
		~HandleRequest();

		HandleRequest &operator=(const HandleRequest &);

		static bool			isDirectory(const std::string& path);
		static std::string	listingBody(const std::string& location);

	public:
		static std::string	handleGet(const std::string& location);
		static std::string	handleDelete(const std::string& location);
		static std::string	handlePost(const std::string& location, std::vector<unsigned char>& content);
};



#endif //HANDLEREQUEST_HPP
