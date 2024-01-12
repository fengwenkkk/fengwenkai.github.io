#include "g_include.h"

#ifdef USE_GOS_UNITTEST
TEST_CASE("gos::EscapeCharUtility �򵥲��� ", "[gos][EscapeCharUtility]")
{
    SECTION("��ͨ���")
    {
        std::string str("Hello World!");
        REQUIRE(str == gos::DecodeEscapeChar(gos::EncodeEscapeChar(str)));
    }

    SECTION("ת���ַ�")
    {
        std::string str("\\");
        REQUIRE(str == gos::DecodeEscapeChar(gos::EncodeEscapeChar(str)));
    }

    SECTION("˫����")
    {
        std::string str("\"");
        REQUIRE(str == gos::DecodeEscapeChar(gos::EncodeEscapeChar(str)));
    }

    SECTION("���������ַ������")
    {
        std::string str("��~!@#$%^&*()_+{}|:<>?");
        REQUIRE(str == gos::DecodeEscapeChar(gos::EncodeEscapeChar(str)));
    }

    SECTION("������")
    {
        std::string str("��~!@#$%^&*()_+{}|:\"<>?\\��~��@#��~%����&*��������+{}|��������������");
        REQUIRE(str == gos::DecodeEscapeChar(gos::EncodeEscapeChar(str)));
    }
}
#endif
