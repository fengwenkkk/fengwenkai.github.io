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

TEST_CASE("gos_atoi64�ļ򵥲���", "[gos][gos_atoi64]")
{
    SECTION("�������")
    {
        SECTION("�������,����")
        {
            long long i64Result = 0;
            char acNum[] = "123";
            REQUIRE(gos_atoi64(acNum, &i64Result));
            REQUIRE(i64Result == 123);
        }
        SECTION("�������, ��")
        {
            long long i64Result = -1;
            char acNum[] = "0";
            REQUIRE(gos_atoi64(acNum, &i64Result));
            REQUIRE(i64Result == 0);
        }
        SECTION("�������,����")
        {
            long long i64Result = 0;
            char acNum[] = "-123";
            REQUIRE(gos_atoi64(acNum, &i64Result));
            REQUIRE(i64Result == -123);
        }
        SECTION("�������,���long longֵ")
        {
            long long i64Result = 0;
            char acNum[32] = "";
            snprintf(acNum, sizeof(acNum) / sizeof(acNum[0]), "%lld", LLONG_MAX);
            REQUIRE(gos_atoi64(acNum, &i64Result));
            REQUIRE(i64Result == LLONG_MAX);
        }
        SECTION("�������,��Сlong longֵ")
        {
            long long i64Result = 0;
            char acNum[32] = "";
            snprintf(acNum, sizeof(acNum) / sizeof(acNum[0]), "%lld", LLONG_MIN);
            /// TODO: �޷���������
            CHECK(gos_atoi64(acNum, &i64Result));
            CHECK(i64Result == LLONG_MIN);
        }

        SECTION("�������: 10000007136")
        {
            long long i64Result = 0;
            REQUIRE(gos_atoi64("10000007136", &i64Result));
            REQUIRE(10000007136 == i64Result);
        }

        SECTION("�������, ��һ���ַ�Ϊ�ո�������ʶ��")
        {
            long long i64Result = 0;
            REQUIRE(gos_atoi64(" 123", &i64Result));
            REQUIRE(123 == i64Result);
            /// ���long longֵ
            char acNum[32] = "";
            snprintf(acNum, sizeof(acNum) / sizeof(acNum[0]), " %lld", LLONG_MAX);
            REQUIRE(gos_atoi64(acNum, &i64Result));
            REQUIRE(i64Result == LLONG_MAX);
            /// ��Сlong longֵ
            /// TODO: ����ط��޷���ȷ����
            snprintf(acNum, sizeof(acNum) / sizeof(acNum[0]), " %lld", LLONG_MIN);
            CHECK(gos_atoi64(acNum, &i64Result));
            CHECK(i64Result == LLONG_MIN);
        }
        SECTION("��������� ��һ���ַ���+���ܹ�����ʶ��")
        {
            long long i64Result = 0;
            REQUIRE(gos_atoi64("+123", &i64Result));
            REQUIRE(i64Result == 123);
        }
        SECTION("�ַ�������ǰ����������")
        {
            long long i64Result = 0;
            REQUIRE(gos_atoi64("0123", &i64Result));
            REQUIRE(i64Result == 123);
            REQUIRE(gos_atoi64("0000000000000456", &i64Result));
            REQUIRE(i64Result == 456);
        }
        SECTION("�ַ���ǰ����ֿո�����")
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

    SECTION("�쳣���")
    {
        SECTION("��ֵ��� LLONG_MAX + 1")
        {
            long long i64Result = 0;
            char acNum[32] = "";
            snprintf(acNum, sizeof(acNum) / sizeof(acNum[0]), "%llu", (unsigned long long)LLONG_MAX + 1);
            REQUIRE(!gos_atoi64(acNum, &i64Result));
        }
        SECTION("��ֵ��� LLONG_MIN - 1, �����һ��������ֵ���޷���������")
        {
            long long i64Result = 0;
            char acNum[32] = "";
            snprintf(acNum, sizeof(acNum) / sizeof(acNum[0]), "%llu", (unsigned long long)LLONG_MIN - 1);
            REQUIRE(gos_atoi64(acNum, &i64Result));
            REQUIRE(i64Result == LLONG_MIN - 1);
        }
        SECTION("��ָ��")
        {
            long long i64Result = 0;
            REQUIRE(!gos_atoi64(NULL, &i64Result));
            REQUIRE(!gos_atoi64("123", NULL));
            REQUIRE(!gos_atoi64(NULL, NULL));
        }
        SECTION("���ַ���")
        {
            long long i64Result = 0;
            REQUIRE(!gos_atoi64("", &i64Result));
        }
        SECTION("�ַ����쳣, һ�������ո� ���쳣����һ������0")
        {
            long long i64Result = 123;
            REQUIRE(gos_atoi64(" ", &i64Result));
            REQUIRE(i64Result == 0);
            i64Result = 123;
            REQUIRE(gos_atoi64("   ", &i64Result));
            REQUIRE(i64Result == 0);
        }
        SECTION("�ַ����쳣�� �Ƿ��ַ�")
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
