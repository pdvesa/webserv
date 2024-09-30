//
// Created by jcayot on 9/23/24.
//

#include <iostream>
#include <Parsing.hpp>

std::string Parsing::extractBlock(std::string& string, const std::string& blockName) {
	const size_t	pos = string.find(blockName);

	if (pos == std::string::npos)
		throw std::runtime_error("Could not find block " + blockName);

	size_t	openBracket = pos + blockName.size();
	std::string	blockContent = extractBracketLayer(string, openBracket);
	std::string remaining = string.substr(0, pos) + string.substr(openBracket + blockName.size() + 2, string.length() - (openBracket + blockName.size() + 2));
	string = remaining;
	return (blockContent);
}

std::string Parsing::extractBracketLayer(std::string& string, size_t start) {
	size_t	openBracket = start;
	while (string[openBracket] != '{' && openBracket < string.length()) {
		if (!std::isspace(string[openBracket]) && string[openBracket] != '\n')
			throw std::runtime_error("Argument string must start with open bracket");
		openBracket++;
	}
	if (openBracket >= string.length())
		throw std::runtime_error("Argument string must start with open bracket");

	size_t	closeBracket = openBracket + 1;
	int		bracketIndex = 0;
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
	std::string	extracted = string.substr(openBracket + 1, closeBracket - openBracket - 1);
	std::string	remaining = string.substr(0, openBracket) + string.substr(closeBracket + 1, string.length() - (closeBracket + 1));
	string = remaining;
	return (extracted);
}
