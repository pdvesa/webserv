//
// Created by jules on 07/10/2024.
//

#include <gtest/gtest.h>
#include <Parsing.hpp>

TEST(ParsingExtractBlockTestSuite, SimpleBlock) {
	std::string testString = "block{content}";
	std::string expectedString = "content";
	std::string result = Parsing::extractBlock(testString, "block");
	ASSERT_STREQ(expectedString.c_str(), result.c_str());
	ASSERT_STREQ(testString.c_str(), "");
}

TEST(ParsingExtractBlockTestSuite, SimpleBlockNewLine) {
	std::string testString = "block\n{content}";
	std::string expectedString = "content";
	std::string result = Parsing::extractBlock(testString, "block");
	ASSERT_STREQ(expectedString.c_str(), result.c_str());
	ASSERT_STREQ(testString.c_str(), "");
}

TEST(ParsingExtractBlockTestSuite, BlockAfterContentNewLine) {
	std::string testString = "other{GHFDGJH}\nblock\n{content}";
	std::string expectedString = "content";
	std::string result = Parsing::extractBlock(testString, "block");
	ASSERT_STREQ(expectedString.c_str(), result.c_str());
	ASSERT_STREQ(testString.c_str(), "other{GHFDGJH}\n");
}

TEST(ParsingExtractBlockTestSuite, ComplexFileBlock) {
	std::string testString = "    server {\n"
		"# configuration of HTTP virtual server 1\n"
		"location /one {\n"
		"# configuration for processing URIs starting with '/one'\n"
				"}\n"
			"location /two {\n"
		"# configuration for processing URIs starting with '/two'\n"
			"}\n"
		"}\n"

		"server {\n"
		"# configuration of HTTP virtual server 2\n"
		"}\n";

	std::string expectedString = "\n# configuration of HTTP virtual server 1\n"
		"location /one {\n"
		"# configuration for processing URIs starting with '/one'\n"
				"}\n"
			"location /two {\n"
		"# configuration for processing URIs starting with '/two'\n"
			"}\n";
	std::string result = Parsing::extractBlock(testString, "server");
	ASSERT_STREQ(expectedString.c_str(), result.c_str());
	ASSERT_STREQ(testString.c_str(), "    \nserver {\n"
		"# configuration of HTTP virtual server 2\n"
		"}\n");
}

TEST(ParsingExtractBlockTestSuite, BlockNotInString) {
	std::string testString = "other{GHFDGJH}\nblock\n{content}";
	ASSERT_THROW(Parsing::extractBlock(testString, "server"), std::runtime_error);
}

TEST(ParsingExtractBlockTestSuite, BlockNotInScope) {
	std::string testString = "other{GHFDGJH}\nblock\n{content\nserver{}}";
	std::string expectedString = "content";
	ASSERT_THROW(Parsing::extractBlock(testString, "server"), std::runtime_error);
}

TEST(ParsingExtractBlockTestSuite, NoOpenBracket) {
	std::string testString = "other{GHFDGJH}\nblock\ncontent";
	ASSERT_THROW(Parsing::extractBlock(testString, "block"), std::runtime_error);
}