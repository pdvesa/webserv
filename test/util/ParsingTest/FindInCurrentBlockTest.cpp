//
// Created by jules on 07/10/2024.
//

#include <gtest/gtest.h>
#include <Parsing.hpp>

TEST(ParsingFindInCurrentBlockTestSuite, SimpleVar) {
	std::string	testString = "var";
	ASSERT_EQ(Parsing::findInCurrentBlock(testString, "var"), 0);
}

TEST(ParsingFindInCurrentBlockTestSuite, VarSomewhere) {
	std::string	testString = "ufaenj\nuewq\nfyrv\n{WEFARgaf}vsrwr{fsdvsv{Vsf}}var erwfva {wver}";
	ASSERT_EQ(Parsing::findInCurrentBlock(testString, "var"), 45);
}

TEST(ParsingFindInCurrentBlockTestSuite, VarSomewhereAndOutOfScope) {
	std::string	testString = "ufaenj\nuewq\nfyrv\n{WEF\nvar ARgaf}vsrwr{fsdvsv{Vsf}}var erwfva {wver}";
	ASSERT_EQ(Parsing::findInCurrentBlock(testString, "var"), 50);
}

TEST(ParsingFindInCurrentBlockTestSuite, VarOutOfScope) {
	std::string	testString = "ufaenj\nuewq\nfyrv\n{WEF\nvar ARgaf}vsrwr{fsdvsv{Vsf}} erwfva {wver}";
	ASSERT_EQ(Parsing::findInCurrentBlock(testString, "var"), std::string::npos);
}

TEST(ParsingFindInCurrentBlockTestSuite, VarAfterNewLine) {
	std::string	testString = "ufaenj\nvar\nfyrv";
	ASSERT_EQ(Parsing::findInCurrentBlock(testString, "var"), 7);
}

TEST(ParsingFindInCurrentBlockTestSuite, VarWithShit) {
	std::string	testString = "ufaenj\nuewq\nfyrv\n{WEF\n ARgaf}vsrwr{fsdvsv{Vsf}}evar erwfva {wver}";
	ASSERT_EQ(Parsing::findInCurrentBlock(testString, "var"), std::string::npos);
}