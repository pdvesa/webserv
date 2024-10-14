//
// Created by jules on 14/10/2024.
//

#include <SpacesClean.hpp>
#include <gtest/gtest.h>

TEST(CleanSpacesTestSuite, SimpleCase) {
	std::string testString = "   test    ";
	std::string excepted = "test";
	std::string result = SpacesClean::cleanSpaces(testString);
	ASSERT_EQ(result, excepted);
}