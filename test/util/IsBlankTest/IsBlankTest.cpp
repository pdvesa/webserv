//
// Created by jules on 16/10/2024.
//

#include <IsBlank.hpp>
#include <gtest/gtest.h>

TEST(IsBlankTestSuite, SimpleTest) {
	EXPECT_EQ(IsBlank::isBlank("	\n"), true);
}