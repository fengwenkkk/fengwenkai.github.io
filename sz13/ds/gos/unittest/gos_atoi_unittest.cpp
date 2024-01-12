#include "g_include.h"
#include <stdint.h>
#include <limits.h>

#ifdef USE_GOS_UNITTEST
TEST_CASE("gos_atoi的简单测试", "[gos][gos_atoi]")
{
    SECTION("正常情况")
    {
        SECTION("正常情况,正数")
        {
            int iResult = 0;
            char acNum[] = "123";
            REQUIRE(gos_atoi(acNum, &iResult));
            REQUIRE(iResult==123);
        }
        SECTION("正常情况, 零")
        {
            int iResult = -1;
            char acNum[] = "0";
            REQUIRE(gos_atoi(acNum, &iResult));
            REQUIRE(iResult==0);
        }
        SECTION("正常情况,负数")
        {
            int iResult = 0;
            char acNum[] = "-123";
            REQUIRE(gos_atoi(acNum, &iResult));
            REQUIRE(iResult==-123);
        }
        SECTION("正常情况,最大int值")
        {
            int iResult = 0;
            char acNum[32] = "";
            snprintf(acNum, sizeof(acNum)/sizeof(acNum[0]), "%d", INT_MAX);
            REQUIRE(gos_atoi(acNum, &iResult));
            REQUIRE(iResult==INT_MAX);
        }
        SECTION("正常情况,最小int值")
        {
            int iResult = 0;
            char acNum[32] = "";
            snprintf(acNum, sizeof(acNum)/sizeof(acNum[0]), "%d", INT_MIN);
            REQUIRE(gos_atoi(acNum, &iResult));
            REQUIRE(iResult==INT_MIN);
        }
        SECTION("正常情况, 第一个字符为空格，能正常识别")
        {
            int iResult = 0;
            char ac[] = "123";
            REQUIRE(gos_atoi(ac, &iResult));
            REQUIRE(123 == iResult);
            /// 最大int值
            char acNum[32] = "";
            snprintf(acNum, sizeof(acNum)/sizeof(acNum[0]), " %d", INT_MAX);
            REQUIRE(gos_atoi(acNum, &iResult));
            REQUIRE(iResult==INT_MAX);
            /// 最小int值
            snprintf(acNum, sizeof(acNum)/sizeof(acNum[0]), " %d", INT_MIN);
            REQUIRE(gos_atoi(acNum, &iResult));
            REQUIRE(iResult==INT_MIN);
        }
        SECTION("正常情况， 第一个字符是+，能够正常识别")
        {
            int iResult = 0;
            char ac[] = "123";
            REQUIRE(gos_atoi(ac, &iResult));
            REQUIRE(iResult == 123);
        }
        SECTION("字符串数字前面是零的情况")
        {
            int iResult = 0;
            char ac0[] = "0123";
            REQUIRE(gos_atoi(ac0, &iResult));
            REQUIRE(iResult == 123);
            char ac[] = "0000000000000456";
            REQUIRE(gos_atoi(ac, &iResult));
            REQUIRE(iResult == 456);
        }
        SECTION("字符串前后出现空格的情况")
        {
            int iResult = 0;
            char ac[] = "+345";
            REQUIRE(gos_atoi(ac, &iResult));
            REQUIRE(iResult == 345);
            char ac1[] = "+567";
            REQUIRE(gos_atoi(ac1, &iResult));
            REQUIRE(iResult == 567);
            char ac2[] = "789 ";
            REQUIRE(gos_atoi(ac2, &iResult));
            REQUIRE(iResult == 789);
            char ac3[] = "890                   ";
            REQUIRE(gos_atoi(ac3, &iResult));
            REQUIRE(iResult == 890);
            char ac4[] = " 123 ";
            REQUIRE(gos_atoi(ac4, &iResult));
            REQUIRE(iResult == 123);
            char ac5[] = "            456      ";
            REQUIRE(gos_atoi(ac5, &iResult));
            REQUIRE(iResult == 456);

        }
    }

    SECTION("异常情况")
    {
        SECTION("数值溢出 INT_MAX + 1, 会给出一个溢出后的值，无法正常运行")
        {
            int iResult = 0;
            char acNum[32] = "";
            snprintf(acNum, sizeof(acNum)/sizeof(acNum[0]), "%lld", (long long)INT_MAX + 1);
            REQUIRE(gos_atoi(acNum, &iResult));
            REQUIRE(iResult == INT_MIN);
        }
        SECTION("数值溢出 INT_MIN - 1, 会给出一个溢出后的值，无法正常运行")
        {
            int iResult = 0;
            char acNum[32] = "";
            snprintf(acNum, sizeof(acNum)/sizeof(acNum[0]), "%lld", (long long)INT_MIN - 1);
            REQUIRE(gos_atoi(acNum, &iResult));
            REQUIRE(iResult == INT_MAX);
        }
        SECTION("空指针")
        {
            int iResult = 0;
            char ac[] = "123";
            REQUIRE(!gos_atoi(NULL, &iResult));
            REQUIRE(!gos_atoi(ac, NULL));
            REQUIRE(!gos_atoi(NULL, NULL));
        }
        SECTION("空字符串")
        {
            int iResult = 0;
            char ac[5] = "";
            REQUIRE(!gos_atoi(ac, &iResult));
        }
        SECTION("字符串异常, 一个或多个空格， 会异常返回一个数字0")
        {
            int iResult = 123;
            char ac[5] = " ";
            REQUIRE(gos_atoi(ac, &iResult));
            REQUIRE(iResult == 0);
            iResult = 123;
            char ac1[] = "   ";
            REQUIRE(gos_atoi(ac1, &iResult));
            REQUIRE(iResult == 0);
        }
        SECTION("字符串异常， 非法字符")
        {
            int iResult = 0;
            char ac[] = ".123";
            REQUIRE(!gos_atoi(ac, &iResult));

            char ac1[] = "0.123";
            REQUIRE(!gos_atoi(ac1, &iResult));

            char ac2[] = "1-23";
            REQUIRE(!gos_atoi(ac2, &iResult));

            char ac3[] = "1+23";
            REQUIRE(!gos_atoi(ac3, &iResult));

            char ac4[] = "12 3";
            REQUIRE(!gos_atoi(ac4, &iResult));

            char ac5[] = "0.123";
            REQUIRE(!gos_atoi(ac5, &iResult));

            char ac6[] = "+ 890";
            REQUIRE(!gos_atoi(ac6, &iResult));

            char ac7[] = "+ 123";
            REQUIRE(!gos_atoi(ac7, &iResult));
        }
    }
}
#endif
