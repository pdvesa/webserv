
#include <gtest/gtest.h>
#include <RouteConfig.hpp>

TEST(RouteConfigFromVariableBlockTestSuite, SimpleVariableBlock) {
    std::list<std::string>  variableBlock = {"/saku/", "\nmethods GET POST\nlisting on\nindex index.html\nroot /svesa/\n"};
    RouteConfig             routeConfig = RouteConfig::fromVariableBlock(variableBlock);
    ASSERT_TRUE(routeConfig.getGET());
    ASSERT_TRUE(routeConfig.getPOST());
    ASSERT_FALSE(routeConfig.getDELETE());
    ASSERT_TRUE(routeConfig.getListing());
    ASSERT_STREQ(routeConfig.getLocation().c_str(), "/saku/");
    ASSERT_STREQ(routeConfig.getRootDir().c_str(), "/svesa/");
}

TEST(RouteConfigFromVariableBlockTestSuite, OtherSimpleVariableBlock) {
    std::list<std::string>  variableBlock = {"/place/", "\nmethods DELETE POST\nindex index.html\nlisting off\nroot /pasvesa/\n"};
    RouteConfig             routeConfig = RouteConfig::fromVariableBlock(variableBlock);
    ASSERT_FALSE(routeConfig.getGET());
    ASSERT_TRUE(routeConfig.getPOST());
    ASSERT_TRUE(routeConfig.getDELETE());
    ASSERT_FALSE(routeConfig.getListing());
    ASSERT_STREQ(routeConfig.getLocation().c_str(), "/place/");
    ASSERT_STREQ(routeConfig.getRootDir().c_str(), "/pasvesa/");
}

TEST(RouteConfigFromVariableBlockTestSuite, InvalidConfigFile) {
    std::list<std::string>  variableBlock = {"/place/", "\nlisting off\nroot /pasvesa/\n"};
    ASSERT_THROW(RouteConfig::fromVariableBlock(variableBlock), Parsing::VariableNotFoundException);
}

TEST(RouteConfigFromVariableBlockTestSuite, Redirection) {
    std::list<std::string>  variableBlock = {"/place/", "\nreturn 302 /saku/"};
    RouteConfig             routeConfig = RouteConfig::fromVariableBlock(variableBlock);
    ASSERT_STREQ(routeConfig.getRedirection().path.c_str(), "/saku/");
    ASSERT_EQ(routeConfig.getRedirection().code, 302);
}