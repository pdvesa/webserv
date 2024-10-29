//
// Created by jules on 09/10/2024.
//

#include <CppSplit.hpp>

std::vector<std::string> CppSplit::cppSplit(const std::string& string, char delim) {
	std::vector<std::string> result;
	std::stringstream stream(string);
	std::string item;

	while (std::getline(stream, item, delim)) {
		if (!item.empty())
			result.push_back(item);
	}
	return (result);
}
