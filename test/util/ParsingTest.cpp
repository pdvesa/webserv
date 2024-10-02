//
// Created by jcayot on 9/23/24.
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
	ASSERT_THROW(Parsing::extractBracketLayer(testString, 0), std::runtime_error);
}

TEST(ParsingExtractBracketLayerTestSuite, UnclosedDoubleBracket) {
	std::string testString = "{{string}";
	ASSERT_THROW(Parsing::extractBracketLayer(testString, 0), std::runtime_error);
}

TEST(ParsingExtractBracketLayerTestSuite, NotABracketStart) {
	std::string testString = "qwewre{string}";
	ASSERT_THROW(Parsing::extractBracketLayer(testString, 0), std::runtime_error);
}

TEST(ParsingExtractBracketLayerTestSuite, offsetStart) {
	std::string testString = "qwer\n{string}";
	std::string expectedString = "string";
	std::string result = Parsing::extractBracketLayer(testString, 5);
	ASSERT_STREQ(expectedString.c_str(), result.c_str());
	ASSERT_STREQ(testString.c_str(), "qwer\n");
}

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
	ASSERT_THROW(Parsing::extractQuoteContent(testString, '"', 0), std::runtime_error);
}

TEST(ParsingExtractQuoteContentTestSuite, UnclosedQuote) {
	std::string testString = "\"string";
	ASSERT_THROW(Parsing::extractQuoteContent(testString, '"', 0), std::runtime_error);
}