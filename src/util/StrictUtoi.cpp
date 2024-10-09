//
// Created by jules on 09/10/2024.
//

#include <stdexcept>
#include <StrictUtoi.hpp>

u_int StrictUtoi::strictUtoi(const std::string& str) {
	size_t	treatedChar;

	u_int result = std::stoul(str, &treatedChar, 10);
	if (treatedChar != str.size())
		throw std::invalid_argument("str is not a number");
	return (result);
}
