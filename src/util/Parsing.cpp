//
// Created by jcayot on 9/23/24.
//

#include <Parsing.hpp>

const std::string	Parsing::VALID_NAME_CHAR = "abcdefghijklmnopqrstuvwxyz";

std::string	Parsing::extractBlock(std::string& string, const std::string& blockName) {
	const size_t	pos = findInCurrentBlock(string, blockName);

	if (pos == std::string::npos)
		throw std::runtime_error("Could not find block : " + blockName);

	size_t	openBracket = pos + blockName.size();
		while (string[openBracket] != '{' && openBracket < string.length()) {
		if (!std::isspace(string[openBracket]) && string[openBracket] != '\n')
			throw std::runtime_error("Argument string must contain open bracket");
		openBracket++;
	}
	if (openBracket >= string.length())
		throw std::runtime_error("Argument string must contain open bracket");

	std::string	blockContent = extractBracketLayer(string, openBracket);
	std::string	remaining = string.substr(0, pos) + string.substr(openBracket, string.size() - (openBracket));
	string = remaining;
	return (blockContent);
}

std::string	Parsing::extractVariable(std::string& string, const std::string& variableName) {
	const size_t	pos = findInCurrentBlock(string, variableName);

	if (pos == std::string::npos)
		throw std::runtime_error("Could not find variable : " + variableName);
	size_t	valuePos = pos + variableName.size();
	while (std::isspace(string[valuePos]))
		valuePos++;
	if (valuePos >= string.length())
		throw std::runtime_error("Could not find value for variable : " + variableName);

	std::string	extracted;
	if (string[valuePos] == '"' || string[valuePos] == '\'')
		extracted = extractQuoteContent(string, string[valuePos], valuePos);
	else
		extracted = extractWord(string, valuePos);

	std::string	remaining = string.substr(0, pos);
	if (valuePos < string.length())
		remaining += string.substr(valuePos - 1, string.length() - (valuePos - 1));
	string = remaining;
	return (extracted);
}

u_int	Parsing::extractInteger(std::string& string, const std::string& variableName) {
	std::string	varString = extractVariable(string, variableName);
	return (StrictUtoi::strictUtoi(varString));
}

std::string	Parsing::extractBracketLayer(std::string& string, size_t start) {
	size_t	closeBracket = start + 1;
	int		bracketIndex = 0;

	if (start >= string.length())
		throw std::out_of_range("Start must be inferior to string.length()");
	if (string[start] != '{')
		throw std::runtime_error("Argument string must start with open bracket");

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
		throw std::runtime_error("Unclosed bracketLayer");

	std::string	extracted = string.substr(start + 1, closeBracket - start - 1);

	std::string	remaining = string.substr(0, start) + string.substr(closeBracket + 1, string.length() - (closeBracket + 1));
	string = remaining;

	return (extracted);
}

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

std::string	Parsing::extractWord(std::string& string, size_t start) {
	size_t	pos = start;

	if (start == 0)
		throw std::runtime_error("Start must be above 0");
	if (start > string.length())
		throw std::out_of_range("Start must be inferior or equal to string.length()");

	while (!isSpecialChar(string[pos]) && string[pos] != ';' && pos < string.length())
		pos++;
	std::string	extracted = string.substr(start, pos - start);

	std::string	remaining = string.substr(0, start - 1) + string.substr(pos, string.length() - pos);
	string = remaining;

	return (extracted);
}

std::string	Parsing::extractQuoteContent(std::string& string, char quote, size_t start) {
	size_t	pos = start;

	if (start >= string.length())
		throw std::out_of_range("Start must be inferior to string.length()");
	if (string[pos] != quote)
		throw std::runtime_error("Argument string must start with " + quote);
	while (++pos < string.length()) {
		if (string[pos] == quote)
			break ;
	}
	if (pos >= string.length())
		throw std::runtime_error(quote + " must be closed");

	std::string	extracted = string.substr(start + 1, pos - start - 1);

	std::string	remaining = string.substr(0, start) + string.substr(pos + 1, string.length() - (pos + 1));
	string = remaining;

	return (extracted);
}

bool	Parsing::isSpecialChar(char c) {
	const std::string	SPECIALS = "{}\n'\"";

	if (SPECIALS.find(c) != std::string::npos)
		return (true);
	return (false);
}
