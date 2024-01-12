#include "g_include.h"

#ifdef USE_GOS_UNITTEST
TEST_CASE("gos_atou的简单测试", "[gos][gos_atou]")
{
    SECTION("正常情况")
    {
        SECTION("正常情况,正数")
        {
            unsigned ulResult = 0;
            char acNum[] = "123";
            REQUIRE(gos_atou(acNum, &ulResult));
            REQUIRE(ulResult==123);
        }
        SECTION("正常情况, 零")
        {
            unsigned ulResult = 123;
            char acNum[] = "0";
            REQUIRE(gos_atou(acNum, &ulResult));
            REQUIRE(ulResult==0);
        }
        SECTION("正常情况,最大unsigned值")
        {
            unsigned ulResult = 0;
            char acNum[32] = "";
            snprintf(acNum, sizeof(acNum)/sizeof(acNum[0]), "%u", UINT_MAX);
            REQUIRE(gos_atou(acNum, &ulResult));
            REQUIRE(ulResult==UINT_MAX);
        }
        SECTION("正常情况,最小unsigned值")
        {
            unsigned ulResult = 123;
            char acNum[32] = "";
            snprintf(acNum, sizeof(acNum)/sizeof(acNum[0]), "%u", 0);
            REQUIRE(gos_atou(acNum, &ulResult));
            REQUIRE(ulResult==0);
        }
        SECTION("正常情况, 第一个字符为空格，能正常识别")
        {
            unsigned ulResult = 0;
            REQUIRE(gos_atou(" 123", &ulResult));
            REQUIRE(123 == ulResult);
            /// 最大unsigned值, 无法正常解析
            char acNum[32] = "";
            snprintf(acNum, sizeof(acNum)/sizeof(acNum[0]), " %u", UINT_MAX);
            REQUIRE(gos_atou(acNum, &ulResult));

            /// 最小unsigned值
            snprintf(acNum, sizeof(acNum)/sizeof(acNum[0]), " %d", 0);
            REQUIRE(gos_atou(acNum, &ulResult));
            REQUIRE(ulResult==0);
        }
        SECTION("正常情况， 第一个字符是+，能够正常识别")
        {
            unsigned ulResult = 0;
            REQUIRE(gos_atou("+123", &ulResult));
            REQUIRE(ulResult == 123);
        }
        SECTION("字符串数字前面是零的情况")
        {
            unsigned ulResult = 0;
            REQUIRE(gos_atou("0123", &ulResult));
            REQUIRE(ulResult == 123);
            REQUIRE(gos_atou("0000000000000456", &ulResult));
            REQUIRE(ulResult == 456);
        }
        SECTION("字符串前后出现空格的情况")
        {
            unsigned ulResult = 0;
            REQUIRE(gos_atou("+345 ", &ulResult));
            REQUIRE(ulResult == 345);
            REQUIRE(gos_atou(" +567", &ulResult));
            REQUIRE(ulResult == 567);
            REQUIRE(gos_atou("789 ", &ulResult));
            REQUIRE(ulResult == 789);
            REQUIRE(gos_atou("890         ", &ulResult));
            REQUIRE(ulResult == 890);
            REQUIRE(gos_atou(" 123 ", &ulResult));
            REQUIRE(ulResult == 123);
            REQUIRE(gos_atou("                 456            ", &ulResult));
            REQUIRE(ulResult == 456);
        }
    }

    SECTION("异常情况")
    {
        SECTION("负数")
        {
            unsigned ulResult = 0;
            char acNum[] = "-123";
            REQUIRE(!gos_atou(acNum, &ulResult));
            /// REQUIRE(ulResult==-123);
        }

        SECTION("数值溢出 UINT_MAX + 1")
        {
            unsigned ulResult = 0;
            char acNum[32] = "";
            snprintf(acNum, sizeof(acNum)/sizeof(acNum[0]), "%llu", (unsigned long long)UINT_MAX + 1);
            REQUIRE(!gos_atou(acNum, &ulResult));
            /// REQUIRE(ulResult == UINT_MAX + 1);
        }
        SECTION("数值溢出 -1")
        {
            unsigned ulResult = 0;
            char acNum[32] = "";
            snprintf(acNum, sizeof(acNum)/sizeof(acNum[0]), "%llu", (unsigned long long)0 - 1);
            REQUIRE(!gos_atou(acNum, &ulResult));
            /// REQUIRE(ulResult == 0 - 1);
        }
        SECTION("空指针")
        {
            unsigned ulResult = 0;
            REQUIRE(!gos_atou(NULL, &ulResult));
            REQUIRE(!gos_atou("123", NULL));
            REQUIRE(!gos_atou(NULL, NULL));
        }
        SECTION("空字符串")
        {
            unsigned ulResult = 0;
            REQUIRE(!gos_atou("", &ulResult));
        }
        SECTION("字符串异常, 一个或多个空格， 会异常返回一个数字0")
        {
            unsigned ulResult = 123;
            REQUIRE(gos_atou(" ", &ulResult));
            REQUIRE(ulResult == 0);
            ulResult = 123;
            REQUIRE(gos_atou("   ", &ulResult));
            REQUIRE(ulResult == 0);
        }
        SECTION("字符串异常， 非法字符")
        {
            unsigned ulResult = 0;
            REQUIRE(!gos_atou(".123", &ulResult));
            REQUIRE(!gos_atou("0.123", &ulResult));

            REQUIRE(!gos_atou("1-23", &ulResult));
            REQUIRE(!gos_atou("1+23", &ulResult));
            REQUIRE(!gos_atou("12 3", &ulResult));

            REQUIRE(!gos_atou("0.123", &ulResult));
            REQUIRE(!gos_atou("+ 890", &ulResult));
            REQUIRE(!gos_atou(" + 123", &ulResult));
        }
    }
}
#endif
