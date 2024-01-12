#include "g_include.h"
#include <limits.h>

#ifdef USE_GOS_UNITTEST

#ifdef __GOS_USE_GNU__
#pragma GCC diagnostic ignored "-Wwrite-strings"
#pragma GCC diagnostic ignored "-Woverflow"
#endif

#ifdef __GOS_USE_VS__
#pragma warning(push)
#pragma warning(disable : 4307)
#endif

TEST_CASE("gos_atoi64的简单测试", "[gos][gos_atoi64]")
{
    SECTION("正常情况")
    {
        SECTION("正常情况,正数")
        {
            long long i64Result = 0;
            char acNum[] = "123";
            REQUIRE(gos_atoi64(acNum, &i64Result));
            REQUIRE(i64Result == 123);
        }
        SECTION("正常情况, 零")
        {
            long long i64Result = -1;
            char acNum[] = "0";
            REQUIRE(gos_atoi64(acNum, &i64Result));
            REQUIRE(i64Result == 0);
        }
        SECTION("正常情况,负数")
        {
            long long i64Result = 0;
            char acNum[] = "-123";
            REQUIRE(gos_atoi64(acNum, &i64Result));
            REQUIRE(i64Result == -123);
        }
        SECTION("正常情况,最大long long值")
        {
            long long i64Result = 0;
            char acNum[32] = "";
            snprintf(acNum, sizeof(acNum) / sizeof(acNum[0]), "%lld", LLONG_MAX);
            REQUIRE(gos_atoi64(acNum, &i64Result));
            REQUIRE(i64Result == LLONG_MAX);
        }
        SECTION("正常情况,最小long long值")
        {
            long long i64Result = 0;
            char acNum[32] = "";
            snprintf(acNum, sizeof(acNum) / sizeof(acNum[0]), "%lld", LLONG_MIN);
            /// TODO: 无法正常解析
            CHECK(gos_atoi64(acNum, &i64Result));
            CHECK(i64Result == LLONG_MIN);
        }

        SECTION("正常情况: 10000007136")
        {
            long long i64Result = 0;
            REQUIRE(gos_atoi64("10000007136", &i64Result));
            REQUIRE(10000007136 == i64Result);
        }

        SECTION("正常情况, 第一个字符为空格，能正常识别")
        {
            long long i64Result = 0;
            REQUIRE(gos_atoi64(" 123", &i64Result));
            REQUIRE(123 == i64Result);
            /// 最大long long值
            char acNum[32] = "";
            snprintf(acNum, sizeof(acNum) / sizeof(acNum[0]), " %lld", LLONG_MAX);
            REQUIRE(gos_atoi64(acNum, &i64Result));
            REQUIRE(i64Result == LLONG_MAX);
            /// 最小long long值
            /// TODO: 这个地方无法正确解析
            snprintf(acNum, sizeof(acNum) / sizeof(acNum[0]), " %lld", LLONG_MIN);
            CHECK(gos_atoi64(acNum, &i64Result));
            CHECK(i64Result == LLONG_MIN);
        }
        SECTION("正常情况， 第一个字符是+，能够正常识别")
        {
            long long i64Result = 0;
            REQUIRE(gos_atoi64("+123", &i64Result));
            REQUIRE(i64Result == 123);
        }
        SECTION("字符串数字前面是零的情况")
        {
            long long i64Result = 0;
            REQUIRE(gos_atoi64("0123", &i64Result));
            REQUIRE(i64Result == 123);
            REQUIRE(gos_atoi64("0000000000000456", &i64Result));
            REQUIRE(i64Result == 456);
        }
        SECTION("字符串前后出现空格的情况")
        {
            long long i64Result = 0;
            REQUIRE(gos_atoi64("+345 ", &i64Result));
            REQUIRE(i64Result == 345);
            REQUIRE(gos_atoi64(" +567", &i64Result));
            REQUIRE(i64Result == 567);
            REQUIRE(gos_atoi64("789 ", &i64Result));
            REQUIRE(i64Result == 789);
            REQUIRE(gos_atoi64("890         ", &i64Result));
            REQUIRE(i64Result == 890);
            REQUIRE(gos_atoi64(" 123 ", &i64Result));
            REQUIRE(i64Result == 123);
            REQUIRE(gos_atoi64("                 456            ", &i64Result));
            REQUIRE(i64Result == 456);
        }
    }

    SECTION("异常情况")
    {
        SECTION("数值溢出 LLONG_MAX + 1")
        {
            long long i64Result = 0;
            char acNum[32] = "";
            snprintf(acNum, sizeof(acNum) / sizeof(acNum[0]), "%llu", (unsigned long long)LLONG_MAX + 1);
            REQUIRE(!gos_atoi64(acNum, &i64Result));
        }
        SECTION("数值溢出 LLONG_MIN - 1, 会给出一个溢出后的值，无法正常运行")
        {
            long long i64Result = 0;
            char acNum[32] = "";
            snprintf(acNum, sizeof(acNum) / sizeof(acNum[0]), "%llu", (unsigned long long)LLONG_MIN - 1);
            REQUIRE(gos_atoi64(acNum, &i64Result));
            REQUIRE(i64Result == LLONG_MIN - 1);
        }
        SECTION("空指针")
        {
            long long i64Result = 0;
            REQUIRE(!gos_atoi64(NULL, &i64Result));
            REQUIRE(!gos_atoi64("123", NULL));
            REQUIRE(!gos_atoi64(NULL, NULL));
        }
        SECTION("空字符串")
        {
            long long i64Result = 0;
            REQUIRE(!gos_atoi64("", &i64Result));
        }
        SECTION("字符串异常, 一个或多个空格， 会异常返回一个数字0")
        {
            long long i64Result = 123;
            REQUIRE(gos_atoi64(" ", &i64Result));
            REQUIRE(i64Result == 0);
            i64Result = 123;
            REQUIRE(gos_atoi64("   ", &i64Result));
            REQUIRE(i64Result == 0);
        }
        SECTION("字符串异常， 非法字符")
        {
            long long i64Result = 0;
            REQUIRE(!gos_atoi64(".123", &i64Result));
            REQUIRE(!gos_atoi64("0.123", &i64Result));

            REQUIRE(!gos_atoi64("1-23", &i64Result));
            REQUIRE(!gos_atoi64("1+23", &i64Result));
            REQUIRE(!gos_atoi64("12 3", &i64Result));

            REQUIRE(!gos_atoi64("0.123", &i64Result));
            REQUIRE(!gos_atoi64("+ 890", &i64Result));
            REQUIRE(!gos_atoi64(" + 123", &i64Result));
        }
    }
}
#ifdef __GOS_USE_VS__
#pragma warning(pop)
#endif
#endif
