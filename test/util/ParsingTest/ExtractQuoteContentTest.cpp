//
// Created by jules on 07/10/2024.
//

#include <gtest/gtest.h>
#include <Parsing.hpp>

TEST(ParsingExtractQuoteContentTestSuite, SimpleQuotes) {
	std::string testString = "\"string\"";
	std::string expectedString = "string";
	std::string	result = Parsing::extractQuoteContent(testString, '"', 0);
	ASSERT_STREQ(expectedString.c_str(), result.c_str());
	ASSERT_STREQ(testString.c_str(), "");
}

TEST(ParsingExtractQuoteContentTestSuite, EmptyQuotes) {
	std::string testString = "\"\"";
	std::string expectedString = "";
	std::string	result = Parsing::extractQuoteContent(testString, '"', 0);
	ASSERT_STREQ(expectedString.c_str(), result.c_str());
	ASSERT_STREQ(testString.c_str(), "");
}

TEST(ParsingExtractQuoteContentTestSuite, LongString) {
	std::string testString = "\"ssuthgjkubfujgjtr84jjk789urythggrewrgrbnuy48w489gr4w\nsdhtyjwfe		trrh gwrg ^&$^^%2\"";
	std::string expectedString = "ssuthgjkubfujgjtr84jjk789urythggrewrgrbnuy48w489gr4w\nsdhtyjwfe		trrh gwrg ^&$^^%2";
	std::string	result = Parsing::extractQuoteContent(testString, '"', 0);
	ASSERT_STREQ(expectedString.c_str(), result.c_str());
	ASSERT_STREQ(testString.c_str(), "");
}

TEST(ParsingExtractQuoteContentTestSuite, MultipleQuotes) {
	std::string testString = R"("Hello""Who""Are""You")";
	std::string expectedString = "Hello";
	std::string	result = Parsing::extractQuoteContent(testString, '"', 0);
	ASSERT_STREQ(expectedString.c_str(), result.c_str());
	ASSERT_STREQ(testString.c_str(), R"("Who""Are""You")");
}

TEST(ParsingExtractQuoteContentTestSuite, OffsetStart) {
	std::string testString = "       \"string\"";
	std::string expectedString = "string";
	std::string	result = Parsing::extractQuoteContent(testString, '"', 7);
	ASSERT_STREQ(expectedString.c_str(), result.c_str());
	ASSERT_STREQ(testString.c_str(), "       ");
}

TEST(ParsingExtractQuoteContentTestSuite, NotAQuote) {
	std::string testString = "       \"string\"";
	ASSERT_THROW(Parsing::extractQuoteContent(testString, '"', 0), Parsing::InvalidFileFormatException);
}

TEST(ParsingExtractQuoteContentTestSuite, UnclosedQuote) {
	std::string testString = "\"string";
	ASSERT_THROW(Parsing::extractQuoteContent(testString, '"', 0), Parsing::InvalidFileFormatException);
}
