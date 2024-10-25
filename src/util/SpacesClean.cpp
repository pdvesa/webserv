//
// Created by jules on 14/10/2024.
//

#include <SpacesClean.hpp>

std::string SpacesClean::cleanLeadingSpaces(const std::string& input) {
	size_t		i = 0;

	while (i < input.size()) {
		if (!isspace(input[i]))
			break ;
		i++;
	}
	return (input.substr(i, input.size() - i));
}

std::string SpacesClean::cleanTrailingSpaces(const std::string& input) {
	ssize_t		i = input.size() - 1;

	while (i >= 0) {
		if (!isspace(input[i]))
			break ;
		i--;
	}
	return (input.substr(0, i + 1));
}

std::string SpacesClean::cleanSpaces(const std::string& input) {
	std::string	result;

	result = cleanLeadingSpaces(input);
	result = cleanTrailingSpaces(result);
	return (result);
}
