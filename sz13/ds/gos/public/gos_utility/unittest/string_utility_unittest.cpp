#include "g_include.h"

#ifdef USE_GOS_UNITTEST

TEST_CASE("string_erase 的简单测试", "[gos][string_erase]")
{
    std::string str;
    std::string strErase;

    str = "ljafl;ja";
    strErase = "";
    gos::string_erase(strErase, str);
    REQUIRE(str == "ljafl;ja");

    str = "ljafl;ja";
    strErase = ";";
    gos::string_erase(strErase, str);
    REQUIRE(str == "ljaflja");

    str = "";
    strErase = "";
    gos::string_erase(strErase, str);
    REQUIRE(str.empty());

    str = "";
    strErase = "lljjl";
    gos::string_erase(strErase, str);
    REQUIRE(str.empty());

    str = "jljljkljljlkj";
    strErase = ",";
    gos::string_erase(strErase, str);
    REQUIRE(str == "jljljkljljlkj");

    str = "jljlj\r\nkljljlkj\r\n";
    strErase = "\r\n";
    gos::string_erase(strErase, str);
    REQUIRE(str == "jljljkljljlkj");
}

TEST_CASE("string_find_prefix 的简单测试", "[gos][string_find_prefix]")
{
    std::string strTest = "test";
    std::string strSrc = strTest + "\\\\adfadfa\\afdadfa\\adfasdf.text";
    std::string strPrefix;
    gos::string_find_prefix(strSrc, "\\", strPrefix);
    REQUIRE(strPrefix == strTest);
}

TEST_CASE("string_find_suffix 的简单测试", "[gos][string_find_suffix]")
{
    std::string strTest = "test";
    std::string strSrc = "\\adf..adfa\\afdadfa\\adfasdf.." + strTest;
    std::string strSuffix;
    gos::string_find_suffix(strSrc, ".", strSuffix);
    REQUIRE(strSuffix == strTest);
}

#endif