//
// Created by jules on 14/10/2024.
//

#include <list>
#include <Parsing.hpp>
#include <gtest/gtest.h>

TEST(ExtractVariableBlockTest, SimpleCase) {
	std::string testString = "location blablabla{blibloudblou}";
	std::string exceptedVar = "blablabla";
	std::string exceptedBlock = "blibloudblou";
	std::list<std::string> result = Parsing::extractVariableBlock(testString, "location");
	ASSERT_EQ(result.size(), 2);
	ASSERT_EQ(result.front(), exceptedVar);
	ASSERT_EQ(result.back(), exceptedBlock);
}