
#include <gtest/gtest.h>
#include <RouteConfig.hpp>

TEST(RouteConfigFromVariableBlockTestSuite, SimpleVariableBlock) {
    std::string variableBlock = {"\nmethods GET POST\nlisting on\nindex index.html\nroot /svesa/\n"};
    RouteConfig routeConfig = RouteConfig::fromLocationBlock(variableBlock);
    ASSERT_TRUE(routeConfig.getGET());
    ASSERT_TRUE(routeConfig.getPOST());
    ASSERT_FALSE(routeConfig.getDELETE());
    ASSERT_TRUE(routeConfig.getListing());
    ASSERT_STREQ(routeConfig.getRootDir().c_str(), "/svesa/");
}

TEST(RouteConfigFromVariableBlockTestSuite, OtherSimpleVariableBlock) {
    std::string variableBlock = {"\nmethods DELETE POST\nindex index.html\nlisting off\nroot /pasvesa/\n"};
    RouteConfig routeConfig = RouteConfig::fromLocationBlock(variableBlock);
    ASSERT_FALSE(routeConfig.getGET());
    ASSERT_TRUE(routeConfig.getPOST());
    ASSERT_TRUE(routeConfig.getDELETE());
    ASSERT_FALSE(routeConfig.getListing());
    ASSERT_STREQ(routeConfig.getRootDir().c_str(), "/pasvesa/");
}

TEST(RouteConfigFromVariableBlockTestSuite, InvalidConfigFile) {
    std::string variableBlock = {"\nlisting off\nroot /pasvesa/\n"};
    ASSERT_THROW(RouteConfig::fromLocationBlock(variableBlock), Parsing::VariableNotFoundException);
}

TEST(RouteConfigFromVariableBlockTestSuite, Redirection) {
    std::string variableBlock = {"\nreturn 302 /saku/"};
    RouteConfig routeConfig = RouteConfig::fromLocationBlock(variableBlock);
    ASSERT_STREQ(routeConfig.getRedirection().path.c_str(), "/saku/");
    ASSERT_EQ(routeConfig.getRedirection().code, 302);
}