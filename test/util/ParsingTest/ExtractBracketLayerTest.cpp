//
// Created by jules on 07/10/2024.
//

#include <gtest/gtest.h>
#include <Parsing.hpp>

TEST(ParsingExtractBracketLayerTestSuite, SimpleLayer) {
	std::string testString = "{string}";
	std::string expectedString = "string";
	std::string result = Parsing::extractBracketLayer(testString, 0);
	ASSERT_STREQ(expectedString.c_str(), result.c_str());
	ASSERT_STREQ(testString.c_str(), "");
}

TEST(ParsingExtractBracketLayerTestSuite, DoubleLayer) {
	std::string testString = "{{string}}";
	std::string expectedString = "{string}";
	std::string result = Parsing::extractBracketLayer(testString, 0);
	ASSERT_STREQ(expectedString.c_str(), result.c_str());
	ASSERT_STREQ(testString.c_str(), "");
}

TEST(ParsingExtractBracketLayerTestSuite, TripleLayer) {
	std::string testString = "{{{string}}}";
	std::string expectedString = "{{string}}";
	std::string result = Parsing::extractBracketLayer(testString, 0);
	ASSERT_STREQ(expectedString.c_str(), result.c_str());
	ASSERT_STREQ(testString.c_str(), "");
}

TEST(ParsingExtractBracketLayerTestSuite, ComplexString) {
	std::string testString = "{"
	"# Configuration specific to HTTP and affecting all virtual servers"
""
	"server {"
"# configuration of HTTP virtual server 1"
		"location /one {"
"# configuration for processing URIs starting with '/one'"
		"}"
		"location /two {"
"# configuration for processing URIs starting with '/two'"
		"}"
	"}"
""
	"server {"
"# configuration of HTTP virtual server 2"
	"}"
"}"
""
"stream {"
"# Configuration specific to TCP/UDP and affecting all virtual servers"
	"server {"
"# configuration of TCP virtual server 1"
	"}"
"}"
";";
	std::string expectedString = "# Configuration specific to HTTP and affecting all virtual servers"
""
	"server {"
"# configuration of HTTP virtual server 1"
		"location /one {"
"# configuration for processing URIs starting with '/one'"
		"}"
		"location /two {"
"# configuration for processing URIs starting with '/two'"
		"}"
	"}"
""
	"server {"
"# configuration of HTTP virtual server 2"
	"}";
	std::string result = Parsing::extractBracketLayer(testString, 0);
	ASSERT_STREQ(expectedString.c_str(), result.c_str());
	ASSERT_STREQ(testString.c_str(), ""
	"stream {"
	"# Configuration specific to TCP/UDP and affecting all virtual servers"
		"server {"
	"# configuration of TCP virtual server 1"
		"}"
	"}"
	";");
}

TEST(ParsingExtractBracketLayerTestSuite, UnclosedBracket) {
	std::string testString = "{string";
	ASSERT_THROW(Parsing::extractBracketLayer(testString, 0), Parsing::InvalidFileFormatException);
}

TEST(ParsingExtractBracketLayerTestSuite, UnclosedDoubleBracket) {
	std::string testString = "{{string}";
	ASSERT_THROW(Parsing::extractBracketLayer(testString, 0), Parsing::InvalidFileFormatException);
}

TEST(ParsingExtractBracketLayerTestSuite, NotABracketStart) {
	std::string testString = "qwewre{string}";
	ASSERT_THROW(Parsing::extractBracketLayer(testString, 0), Parsing::InvalidFileFormatException);
}

TEST(ParsingExtractBracketLayerTestSuite, offsetStart) {
	std::string testString = "qwer\n{string}";
	std::string expectedString = "string";
	std::string result = Parsing::extractBracketLayer(testString, 5);
	ASSERT_STREQ(expectedString.c_str(), result.c_str());
	ASSERT_STREQ(testString.c_str(), "qwer\n");
}