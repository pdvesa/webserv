//
// Created by jcayot on 9/23/24.
//

#include <Parsing.hpp>
#include <SpacesClean.hpp>

const std::string	Parsing::VALID_NAME_CHAR = "abcdefghijklmnopqrstuvwxyz";

/**
 * Extracts the value of a variable and the content of a block from a string, given the name of the variable and the block.
 *
 * @param string The input string from which to extract the variable value and block content.
 * @param blockName The name of the block to extract.
 * @return A std::list containing the extracted variable value and block content as std::string objects.
 * @throws BlockNotFoundException If the block is not found in the current block of the input string.
 * @throws InvalidFileFormatException If the variable value or block content is not properly formatted.
 *
 * This function first searches for the block with the given name in the current block of the input
 * string using the `findInCurrentBlock` function. If the block is found, the function extracts the value
 * of the variable with the same name using the `extractVariable` function, and extracts the content of
 * the block using the `extractBracketLayer` function. If the block is not found, or if the variable value
 * or block content is not properly formatted, the function throws an exception. The function returns a
 * std::list containing the extracted variable value and block content as std::string objects. The input
 * string is modified to remove the extracted variable and block.
 */

std::list<std::string> Parsing::extractVariableBlock(std::string& string, const std::string& blockName) {
	const size_t	pos = findInCurrentBlock(string, blockName);

	if (pos == std::string::npos)
		throw Parsing::BlockNotFoundException("Could not find block : " + blockName);
	std::list<std::string> result;
	result.push_back(extractVariable(string, blockName));
	result.push_back(extractBracketLayer(string, pos));
	return (result);
}


/**
 * Extracts the content of a block from a string, given the name of the block.
 *
 * @param string The input string from which to extract the block content.
 * @param blockName The name of the block to extract.
 * @return The extracted block content as a std::string.
 * @throws BlockNotFoundException If the block is not found in the current block of the input string.
 * @throws InvalidFileFormatException If the block content is not properly formatted.
 *
 * This function first searches for the block with the given name in the current block of the input
 * string using the `findInCurrentBlock` function. If the block is found, the function extracts its
 * content from the input string using the `extractBracketLayer` function. If the block is not found,
 * or if the block content is not properly formatted, the function throws an exception. The input
 * string is modified to remove the extracted block and its content.
 */

std::string	Parsing::extractBlock(std::string& string, const std::string& blockName) {
	const size_t	pos = findInCurrentBlock(string, blockName);

	if (pos == std::string::npos)
		throw BlockNotFoundException("Could not find block : " + blockName);

	size_t	openBracket = pos + blockName.size();
		while (string[openBracket] != '{' && openBracket < string.length()) {
		if (!std::isspace(string[openBracket]) && string[openBracket] != '\n')
			throw InvalidFileFormatException("Argument string must contain open bracket");
		openBracket++;
	}
	if (openBracket >= string.length())
		throw InvalidFileFormatException("Argument string must contain open bracket");

	std::string	blockContent = extractBracketLayer(string, openBracket);
	std::string	remaining = string.substr(0, pos) + string.substr(openBracket, string.size() - (openBracket));
	string = remaining;
	return (blockContent);
}


/**
 * Extracts the value of a variable from a string, given the name of the variable.
 *
 * @param string The input string from which to extract the variable value.
 * @param variableName The name of the variable to extract.
 * @return The extracted variable value as a std::string.
 * @throws VariableNotFoundException If the variable is not found in the current block of the input string.
 * @throws InvalidFileFormatException If the variable value is not properly formatted.
 *
 * This function first searches for the variable with the given name in the current block of the input
 * string using the `findInCurrentBlock` function. If the variable is found, the function extracts its
 * value from the input string. If the value is enclosed in quotes, the function uses the
 * `extractQuoteContent` function to extract the value. Otherwise, the function uses the `extractWord`
 * function to extract the value. The function then removes any leading or trailing whitespace from the
 * extracted value using the `SpacesClean::cleanSpaces` function. If the variable is not found, or if the
 * variable value is not properly formatted, the function throws an exception. The input string is modified
 * to remove the extracted variable and its value.
 */

std::string	Parsing::extractVariable(std::string& string, const std::string& variableName) {
	const size_t	pos = findInCurrentBlock(string, variableName);

	if (pos == std::string::npos)
		throw VariableNotFoundException("Could not find variable : " + variableName);
	size_t	valuePos = pos + variableName.size();
	while (std::isspace(string[valuePos]))
		valuePos++;
	if (valuePos >= string.length())
		throw InvalidFileFormatException("Could not find value for variable : " + variableName);

	std::string	extracted;
	if (string[valuePos] == '"' || string[valuePos] == '\'')
		extracted = extractQuoteContent(string, string[valuePos], valuePos);
	else
		extracted = extractWord(string, valuePos);

	std::string	remaining = string.substr(0, pos);
	if (valuePos < string.length())
		remaining += string.substr(valuePos - 1, string.length() - (valuePos - 1));
	string = remaining;
	return (SpacesClean::cleanSpaces(extracted));
}


/**
 * Extracts an integer value from a string, given the name of a variable that contains the integer value.
 *
 * @param string The input string from which to extract the integer value.
 * @param variableName The name of the variable that contains the integer value.
 * @return The extracted integer value.
 * @throws std::invalid_argument If the variable value is not a valid integer.
 *
 * This function first extracts the value of the variable with the given name from the input string using
 * the `extractVariable` function. It then converts the extracted value to an integer using the
 * `StrictUtoi::strictUtoi` function. If the extracted value is not a valid integer, the function throws
 * a `std::invalid_argument` exception.
 */

u_int	Parsing::extractInteger(std::string& string, const std::string& variableName) {
	std::string	varString = extractVariable(string, variableName);
	return (StrictUtoi::strictUtoi(varString));
}


/**
 * Extracts the content between a pair of matching curly braces ({ and }) from a string, starting at a
 * specified position.
 *
 * @param string The input string from which to extract the content.
 * @param start The starting position in the input string from which to begin the search for the matching
 *              curly braces.
 * @return The extracted content as a std::string.
 * @throws std::out_of_range If the starting position is greater than or equal to the length of the input
 *         string.
 * @throws InvalidFileFormatException If the input string does not start with an open curly brace, or if
 *         the curly braces are not properly nested.
 *
 * This function searches for a pair of matching curly braces ({ and }) in the input string (`string`),
 * starting at the position specified by the `start` parameter. If the curly braces are found and
 * properly nested, the content between them is extracted and returned as a std::string. If the starting
 * position is out of range, or if the curly braces are not found or properly nested, an exception is
 * thrown. The input string is modified to remove the extracted content and the closing curly brace.
 */

std::string	Parsing::extractBracketLayer(std::string& string, size_t start) {
	size_t	closeBracket = start + 1;
	int		bracketIndex = 0;

	if (start >= string.length())
		throw std::out_of_range("Start must be inferior to string.length()");
	if (string[start] != '{')
		throw InvalidFileFormatException("Argument string must start with open bracket");

	while (closeBracket < string.length()) {
		if (string[closeBracket] == '}') {
			if (bracketIndex == 0)
				break ;
			bracketIndex--;
		} else if (string[closeBracket] == '{')
			bracketIndex++;
		closeBracket++;
	}
	if (closeBracket >= string.length())
		throw InvalidFileFormatException("Unclosed bracketLayer");

	std::string	extracted = string.substr(start + 1, closeBracket - start - 1);

	std::string	remaining = string.substr(0, start) + string.substr(closeBracket + 1, string.length() - (closeBracket + 1));
	string = remaining;

	return (extracted);
}


/**
 * Searches for a given name in the current block of a string.
 *
 * @param string The input string in which to search for the name.
 * @param name The name to search for.
 * @return The position of the first character of the name in the string, or std::string::npos if the name
 *         is not found in the current block.
 *
 * This function searches for the given name in the current block of the input string. A block is defined
 * as the text between a pair of matching curly braces ({ and }). The function keeps track of the current
 * block using a bracket layer counter. If the name is found in the current block, the function returns the
 * position of the first character of the name in the string. If the name is not found in the current
 * block, the function returns std::string::npos.
 */

size_t	Parsing::findInCurrentBlock(const std::string& string, const std::string& name) {
	size_t	pos = 0;
	u_int	bracketLayer = 0;

	while (pos < string.length()) {
		if (string[pos] == '{')
			bracketLayer++;
		else if (string[pos] == '}')
			bracketLayer--;
		else if (bracketLayer == 0) {
			if (string.compare(pos, name.length(), name) == 0)
				return (pos);
		}
		pos++;
	}
	return (std::string::npos);
}


/**
 * Extracts a word from a string, starting at a specified position.
 *
 * @param string The input string from which to extract the word.
 * @param start The starting position in the input string from which to begin the search for the word.
 * @return The extracted word as a std::string.
 * @throws std::out_of_range If the starting position is less than 1 or greater than the length of the input string.
 *
 * This function searches for a word in the input string (`string`), starting at the position specified by the `start` parameter. A word is defined as a sequence of non-special characters that does not contain a semicolon (;). If a word is found, it is extracted and returned as a std::string. If the starting position is out of range, an exception is thrown. The input string is modified to remove the extracted word.
 */

std::string	Parsing::extractWord(std::string& string, size_t start) {
	size_t	pos = start;

	if (start == 0)
		throw std::out_of_range("Start must be above 0");
	if (start > string.length())
		throw std::out_of_range("Start must be inferior or equal to string.length()");

	while (!isSpecialChar(string[pos]) && string[pos] != ';' && pos < string.length())
		pos++;
	std::string	extracted = string.substr(start, pos - start);

	std::string	remaining = string.substr(0, start - 1) + string.substr(pos, string.length() - pos);
	string = remaining;

	return (extracted);
}


/**
 * Extracts the content between a pair of quotes from a given string, starting at a specified position.
 *
 * @param string The input string from which to extract the quoted content.
 * @param quote The quote character that delimits the content to be extracted.
 * @param start The starting position in the input string from which to begin the search for the quoted content.
 * @return The extracted quoted content as a std::string.
 * @throws std::out_of_range If the starting position is greater than or equal to the length of the input string.
 * @throws InvalidFileFormatException If the input string does not start with the specified quote character,
 *         or if the quote character is not closed.
 *
 * This function searches for a pair of quote characters (specified by the `quote` parameter) in the input
 * string (`string`), starting at the position specified by the `start` parameter. If the quote characters
 * are found and properly nested, the content between them is extracted and returned as a std::string. If
 * the starting position is out of range, or if the quote characters are not found or properly nested, an
 * exception is thrown. The input string is modified to remove the extracted content and the closing quote
 * character.
 */

std::string	Parsing::extractQuoteContent(std::string& string, char quote, size_t start) {
	size_t	pos = start;

	if (start >= string.length())
		throw std::out_of_range("Start must be inferior to string.length()");
	if (string[pos] != quote)
		throw InvalidFileFormatException("Argument string must start with " + quote);
	while (++pos < string.length()) {
		if (string[pos] == quote)
			break ;
	}
	if (pos >= string.length())
		throw InvalidFileFormatException(quote + " must be closed");

	std::string	extracted = string.substr(start + 1, pos - start - 1);

	std::string	remaining = string.substr(0, start) + string.substr(pos + 1, string.length() - (pos + 1));
	string = remaining;

	return (extracted);
}


/**
 * Checks if a given character is a special character.
 *
 * @param c The character to check.
 * @return True if the character is a special character, false otherwise.
 *
 * This function checks if the given character is one of the special characters defined in the `SPECIALS`
 * constant string. The special characters are '{', '}', '\n', ''', and '\"'. If the character is found in
 * the `SPECIALS` string, the function returns true. Otherwise, it returns false.
 */

bool	Parsing::isSpecialChar(char c) {
	const std::string	SPECIALS = "{}\n'\"";

	if (SPECIALS.find(c) != std::string::npos)
		return (true);
	return (false);
}
