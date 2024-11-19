//
// Created by jules on 14/11/2024.
//

#ifndef HANDLEREQUEST_HPP
#define HANDLEREQUEST_HPP

#include <string>
#include <fstream>
#include <sstream>


class HandleRequest {
	private:
		HandleRequest();
		HandleRequest(const HandleRequest &);
		~HandleRequest();

		HandleRequest &operator=(const HandleRequest &);

	public:
		static std::string	handleGet(const std::string& location);
		static void			handleDelete(const std::string& location);
		static void			handlePost(const std::string& location, const std::string& content);
};



#endif //HANDLEREQUEST_HPP
