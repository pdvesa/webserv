//
// Created by jules on 16/10/2024.
//

#include <CppSplit.hpp>
#include <gtest/gtest.h>

TEST(CppSplitTestSuite, SimpleTest) {
	std::vector<std::string> expected = {"hello", "world"};
	std::string input = "hello,world";
	char delimiter = ',';

	std::vector<std::string> result = CppSplit::cppSplit(input, delimiter);

	ASSERT_EQ(result.size(), expected.size());
	for (size_t i = 0; i < expected.size(); ++i) {
		EXPECT_EQ(result[i], expected[i]);
	}
}

TEST(CppSplitTestSuite, EmptyStringTest) {
    std::vector<std::string> expected = {};
    std::string input = "";
    char delimiter = ',';

    std::vector<std::string> result = CppSplit::cppSplit(input, delimiter);

    ASSERT_EQ(result.size(), expected.size());
    for (size_t i = 0; i < expected.size(); ++i) {
        EXPECT_EQ(result[i], expected[i]);
    }
}

TEST(CppSplitTestSuite, SingleCharacterTest) {
    std::vector<std::string> expected = {"a"};
    std::string input = "a";
    char delimiter = ',';

    std::vector<std::string> result = CppSplit::cppSplit(input, delimiter);

    ASSERT_EQ(result.size(), expected.size());
    for (size_t i = 0; i < expected.size(); ++i) {
        EXPECT_EQ(result[i], expected[i]);
    }
}

TEST(CppSplitTestSuite, NoDelimiterTest) {
    std::vector<std::string> expected = {"abcdef"};
    std::string input = "abcdef";
    char delimiter = ',';

    std::vector<std::string> result = CppSplit::cppSplit(input, delimiter);

    ASSERT_EQ(result.size(), expected.size());
    for (size_t i = 0; i < expected.size(); ++i) {
        EXPECT_EQ(result[i], expected[i]);
    }
}

TEST(CppSplitTestSuite, LeadingDelimiterTest) {
    std::vector<std::string> expected = { "leading", "delimiter"};
    std::string input = ",leading,delimiter";
    char delimiter = ',';

    std::vector<std::string> result = CppSplit::cppSplit(input, delimiter);

    ASSERT_EQ(result.size(), expected.size());
    for (size_t i = 0; i < expected.size(); ++i) {
        EXPECT_EQ(result[i], expected[i]);
    }
}

TEST(CppSplitTestSuite, TrailingDelimiterTest) {
    std::vector<std::string> expected = {"trailing", "delimiter"};
    std::string input = "trailing,delimiter,";
    char delimiter = ',';

    std::vector<std::string> result = CppSplit::cppSplit(input, delimiter);

    ASSERT_EQ(result.size(), expected.size());
    for (size_t i = 0; i < expected.size(); ++i) {
        EXPECT_EQ(result[i], expected[i]);
    }
}

TEST(CppSplitTestSuite, ConsecutiveDelimitersTest) {
    std::vector<std::string> expected = {"consecutive", "delimiters"};
    std::string input = "consecutive,,delimiters";
    char delimiter = ',';

    std::vector<std::string> result = CppSplit::cppSplit(input, delimiter);

    ASSERT_EQ(result.size(), expected.size());
    for (size_t i = 0; i < expected.size(); ++i) {
        EXPECT_EQ(result[i], expected[i]);
    }
}

TEST(CppSplitTestSuite, SpaceDelimiterTest) {
    std::vector<std::string> expected = {"split", "by", "spaces"};
    std::string input = "split by spaces";
    char delimiter = ' ';

    std::vector<std::string> result = CppSplit::cppSplit(input, delimiter);

    ASSERT_EQ(result.size(), expected.size());
    for (size_t i = 0; i < expected.size(); ++i) {
        EXPECT_EQ(result[i], expected[i]);
    }
}

TEST(CppSplitTestSuite, TabDelimiterTest) {
    std::vector<std::string> expected = {"split", "by", "tabs"};
    std::string input = "split\tby\ttabs";
    char delimiter = '\t';

    std::vector<std::string> result = CppSplit::cppSplit(input, delimiter);

    ASSERT_EQ(result.size(), expected.size());
    for (size_t i = 0; i < expected.size(); ++i) {
        EXPECT_EQ(result[i], expected[i]);
    }
}