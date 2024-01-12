
#include "g_include.h"

#ifdef USE_GOS_UNITTEST

TEST_CASE("输入字符串校验 简单测试用例", "[gos_utility][input_check]")
{
    REQUIRE(gos::IsInputUserNameValid("4894sd9gsgs"));
    REQUIRE(gos::IsInputUserNameValid(""));
    REQUIRE(!gos::IsInputUserNameValid(" "));
    REQUIRE(!gos::IsInputUserNameValid("4894 sd9gsgs"));
    REQUIRE(gos::IsInputIPV4Valid("0.0.0.0"));
    REQUIRE(gos::IsInputIPV4Valid("0.1.1.0"));
    REQUIRE(gos::IsInputIPV4Valid("192.168.1.116"));
    REQUIRE(gos::IsInputIPV4Valid("255.255.255.255"));
    REQUIRE(!gos::IsInputIPV4Valid("256.255.255.255"));
}

#endif