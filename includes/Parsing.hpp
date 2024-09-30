//
// Created by jcayot on 9/23/24.
//

#ifndef PARSING_HPP
#define PARSING_HPP

#include <string>
#include <stdexcept>
#include <cstring>

class Parsing {
	private:
		Parsing();
		Parsing(const Parsing&);

		Parsing& operator=(const Parsing&);

    public:
		static std::string	extractBlock(std::string& string, const std::string& blockName);
		static std::string  extractBracketLayer(std::string& string, size_t start = 0);
};

#endif //PARSING_HPP
