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
		static const std::string	VALID_NAME_CHAR;
		Parsing();
		Parsing(const Parsing&);

		Parsing& operator=(const Parsing&);

		static bool			isSpecialChar(char c);

    public:
		static std::string	extractBlock(std::string& string, const std::string& blockName);
		static size_t		findInCurrentBlock(const std::string& string, const std::string& name);
		static std::string  extractBracketLayer(std::string& string, size_t start = 0);
		static std::string	extractVariable(std::string& string, const std::string& variableName);
		static std::string	extractQuoteContent(std::string& string, char quote, size_t start);
		static std::string	extractWord(std::string& string, size_t start);
};

#endif //PARSING_HPP
