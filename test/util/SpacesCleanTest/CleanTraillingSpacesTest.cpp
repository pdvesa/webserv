//
// Created by jules on 14/10/2024.
//

#include <SpacesClean.hpp>
#include <gtest/gtest.h>

TEST(CleanTraillingSpacesTestSuite, SimpleTest) {
	std::string	testString = "test   ";
	std::string	expectedString = "test";
	std::string	result = SpacesClean::cleanTrailingSpaces(testString);
	ASSERT_EQ(expectedString, result);
}

TEST(CleanTraillingSpacesTestSuite, NoSpaces) {
	std::string	testString = "test";
	std::string	expectedString = "test";
	std::string	result = SpacesClean::cleanTrailingSpaces(testString);
	ASSERT_EQ(expectedString, result);
}

TEST(CleanTraillingSpacesTestSuite, SpacesMiddle) {
	std::string	testString = "te st     ";
	std::string	expectedString = "te st";
	std::string	result = SpacesClean::cleanTrailingSpaces(testString);
	ASSERT_EQ(expectedString, result);
}

TEST(CleanTraillingSpacesTestSuite, OneSpace) {
	std::string	testString = "test ";
	std::string	expectedString = "test";
	std::string	result = SpacesClean::cleanTrailingSpaces(testString);
	ASSERT_EQ(expectedString, result);
}

TEST(CleanTraillingSpacesTestSuite, SpaceStart) {
	std::string	testString = "  test     ";
	std::string	expectedString = "  test";
	std::string	result = SpacesClean::cleanTrailingSpaces(testString);
	ASSERT_EQ(expectedString, result);
}