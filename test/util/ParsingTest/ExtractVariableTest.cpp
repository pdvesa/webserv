//
// Created by jules on 07/10/2024.
//

#include <gtest/gtest.h>
#include <Parsing.hpp>

TEST(ParsingExtractVariableTestSuite, SimpleVar) {
	std::string	testString = "var value";
	std::string	excepted = "value";
	std::string result = Parsing::extractVariable(testString, "var");
	ASSERT_EQ(result, excepted);
	ASSERT_EQ(testString, "");
}

TEST(ParsingExtractVariableTestSuite, SimpleVarInQuotes) {
	std::string	testString = "var\"value\"";
	std::string	excepted = "value";
	std::string result = Parsing::extractVariable(testString, "var");
	ASSERT_EQ(result, excepted);
	ASSERT_EQ(testString, "");
}

TEST(ParsingExtractVariableTestSuite, VarEndLine) {
	std::string	testString = "var value\njfhxdxhfcjhgkvlbnoijpk[liubyerzyxecbiuo";
	std::string	excepted = "value";
	std::string result = Parsing::extractVariable(testString, "var");
	ASSERT_EQ(result, excepted);
	ASSERT_EQ(testString, "\njfhxdxhfcjhgkvlbnoijpk[liubyerzyxecbiuo");
}

TEST(ParsingExtractVariableTestSuite, VarBlock) {
	std::string	testString = "var value{\njfhxdxhfcjhgkvlbnoijpk[liubyerzyxecbiuo}";
	std::string	excepted = "value";
	std::string result = Parsing::extractVariable(testString, "var");
	ASSERT_EQ(result, excepted);
	ASSERT_EQ(testString, "{\njfhxdxhfcjhgkvlbnoijpk[liubyerzyxecbiuo}");
}

TEST(ParsingExtractVariableTestSuite, VarNotInString) {
	std::string	testString = "val value";
	ASSERT_THROW(Parsing::extractVariable(testString, "var"), Parsing::VariableNotFoundException);
}

TEST(ParsingExtractVariableTestSuite, VarNoValue) {
	std::string	testString = "var";
	ASSERT_THROW(Parsing::extractVariable(testString, "var"), Parsing::InvalidFileFormatException);
}