//
// Created by jules on 16/10/2024.
//

#include <IsBlank.hpp>

bool IsBlank::isBlank(const std::string& string) {
	for (int i = 0; i < string.length(); ++i) {
		if (!isspace(string[i]) && string[i] != '\n')
			return (false);
	}
	return (true);
}
