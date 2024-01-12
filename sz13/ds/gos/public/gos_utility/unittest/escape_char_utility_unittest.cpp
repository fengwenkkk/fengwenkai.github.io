#include "g_include.h"

#ifdef USE_GOS_UNITTEST
TEST_CASE("gos::EscapeCharUtility 简单测试 ", "[gos][EscapeCharUtility]")
{
    SECTION("普通情况")
    {
        std::string str("Hello World!");
        REQUIRE(str == gos::DecodeEscapeChar(gos::EncodeEscapeChar(str)));
    }

    SECTION("转义字符")
    {
        std::string str("\\");
        REQUIRE(str == gos::DecodeEscapeChar(gos::EncodeEscapeChar(str)));
    }

    SECTION("双引号")
    {
        std::string str("\"");
        REQUIRE(str == gos::DecodeEscapeChar(gos::EncodeEscapeChar(str)));
    }

    SECTION("不含特殊字符的情况")
    {
        std::string str("·~!@#$%^&*()_+{}|:<>?");
        REQUIRE(str == gos::DecodeEscapeChar(gos::EncodeEscapeChar(str)));
    }

    SECTION("混合情况")
    {
        std::string str("·~!@#$%^&*()_+{}|:\"<>?\\·~！@#￥~%……&*（）——+{}|：“《》？、·");
        REQUIRE(str == gos::DecodeEscapeChar(gos::EncodeEscapeChar(str)));
    }
}
#endif
