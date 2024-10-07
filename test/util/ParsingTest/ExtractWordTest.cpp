//
// Created by jules on 07/10/2024.
//

#include <gtest/gtest.h>
#include <Parsing.hpp>

TEST(ParsingExtractWordTestSuite, SimpleWord) {
	std::string testString = "var value";
	std::string expectedString = "value";
	std::string	result = Parsing::extractWord(testString, 4);
	ASSERT_STREQ(expectedString.c_str(), result.c_str());
	ASSERT_STREQ(testString.c_str(), "var");
}

TEST(ParsingExtractWordTestSuite, NoValue) {
	std::string testString = "var ";
	std::string expectedString = "";
	std::string	result = Parsing::extractWord(testString, 4);
	ASSERT_STREQ(expectedString.c_str(), result.c_str());
	ASSERT_STREQ(testString.c_str(), "var");
}

TEST(ParsingExtractWordTestSuite, LongWeirdWord) {
	std::string testString = " kqerionwt3u748t5h$@%^#&$R#4ctrcsgtvrkgtxdxchbninhkucxyerxrcgv";
	std::string expectedString = "kqerionwt3u748t5h$@%^#&$R#4ctrcsgtvrkgtxdxchbninhkucxyerxrcgv";
	std::string	result = Parsing::extractWord(testString, 1);
	ASSERT_STREQ(expectedString.c_str(), result.c_str());
	ASSERT_STREQ(testString.c_str(), "");
}

TEST(ParsingExtractWordTestSuite, ContentAfterNoSpace) {
	std::string testString = "var value{gabeureewnafungrklsfdkndf}";
	std::string expectedString = "value";
	std::string	result = Parsing::extractWord(testString, 4);
	ASSERT_STREQ(expectedString.c_str(), result.c_str());
	ASSERT_STREQ(testString.c_str(), "var{gabeureewnafungrklsfdkndf}");
}

TEST(ParsingExtractWordTestSuite, ContentAfterSpace) {
	std::string testString = "var value othercontent:P";
	std::string expectedString = "value";
	std::string	result = Parsing::extractWord(testString, 4);
	ASSERT_STREQ(expectedString.c_str(), result.c_str());
	ASSERT_STREQ(testString.c_str(), "var othercontent:P");
}

TEST(ParsingExtractWordTestSuite, startAt0) {
	std::string testString = "var value othercontent:P";
	ASSERT_THROW(Parsing::extractWord(testString, 0), std::runtime_error);
}

TEST(ParsingExtractWordTestSuite, startAboveLen) {
	std::string testString = "var";
	ASSERT_THROW(Parsing::extractWord(testString, 4), std::out_of_range);
}