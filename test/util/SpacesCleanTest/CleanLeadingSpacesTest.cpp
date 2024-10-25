//
// Created by jules on 14/10/2024.
//

#include <SpacesClean.hpp>
#include <gtest/gtest.h>

TEST(CleanLeadingSpacesTestSuite, SimpleTest) {
	std::string	testString = "   test";
	std::string	expectedString = "test";
	std::string	result = SpacesClean::cleanLeadingSpaces(testString);
	ASSERT_EQ(expectedString, result);
}

TEST(CleanLeadingSpacesTestSuite, NoSpaces) {
	std::string	testString = "test";
	std::string	expectedString = "test";
	std::string	result = SpacesClean::cleanLeadingSpaces(testString);
	ASSERT_EQ(expectedString, result);
}

TEST(CleanLeadingSpacesTestSuite, SpacesMiddle) {
	std::string	testString = "    te st";
	std::string	expectedString = "te st";
	std::string	result = SpacesClean::cleanLeadingSpaces(testString);
	ASSERT_EQ(expectedString, result);
}

TEST(CleanLeadingSpacesTestSuite, OneSpace) {
	std::string	testString = " test";
	std::string	expectedString = "test";
	std::string	result = SpacesClean::cleanLeadingSpaces(testString);
	ASSERT_EQ(expectedString, result);
}

TEST(CleanLeadingSpacesTestSuite, SpaceEnd) {
	std::string	testString = " test    ";
	std::string	expectedString = "test    ";
	std::string	result = SpacesClean::cleanLeadingSpaces(testString);
	ASSERT_EQ(expectedString, result);
}