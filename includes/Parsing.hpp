//
// Created by jcayot on 9/23/24.
//

#ifndef PARSING_HPP
#define PARSING_HPP

#include <string>
#include <stdexcept>
#include <cstring>
#include <iostream>

#include <StrictUtoi.hpp>

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
		static u_int		extractInteger(std::string& string, const std::string& variableName);
		static std::string	extractQuoteContent(std::string& string, char quote, size_t start);
		static std::string	extractWord(std::string& string, size_t start);

	class BlockNotFoundException : public std::exception {
		std::string message;

		public:
		const char* what() const noexcept override {
			if (message.empty()) {
				return "Block not found exception";
			} else {
				return message.c_str();
			}
		}

		BlockNotFoundException() = default;

		explicit BlockNotFoundException(const std::string& msg) : message(msg) {}

		~BlockNotFoundException() override = default;
	};

	class VariableNotFoundException : public std::exception {
		std::string message;

		public:
		const char* what() const noexcept override {
			if (message.empty()) {
				return "Variable not found exception";
			} else {
				return message.c_str();
			}
		}

		VariableNotFoundException() = default;

		explicit VariableNotFoundException(const std::string& msg) : message(msg) {}

		~VariableNotFoundException() override = default;
	};

	class InvalidFileFormatException : public std::exception {
		std::string message;

		public:
		const char* what() const noexcept override {
			if (message.empty()) {
				return "Invalid file format exception";
			} else {
				return message.c_str();
			}
		}

		InvalidFileFormatException() = default;

		explicit InvalidFileFormatException(const std::string& msg) : message(msg) {}

		~InvalidFileFormatException() override = default;
	};
};

#endif //PARSING_HPP
