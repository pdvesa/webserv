//
// Created by jules on 21/12/2024.
//

#ifndef HTTP_METHODS_H
# define HTTP_METHODS_H

#include <unordered_map>

static const std::string GET_STR = "GET";
static const std::string POST_STR = "POST";
static const std::string DELETE_STR = "DELETE";

typedef enum e_method {
	GET,
	POST,
	DELETE,
	UNINITIALIZED,
}	t_method;

inline t_method stringToMethod(const std::string& methodStr) {
	static const std::unordered_map<std::string, t_method> methodMap = {
		{GET_STR, GET},
		{POST_STR, POST},
		{DELETE_STR, DELETE},
	};
	auto it = methodMap.find(methodStr);
	if (it != methodMap.end()) {
		return it->second;
	}
	return UNINITIALIZED;  // Default value for unknown or unsupported methods
}

#endif //HTTP_METHODS_H
