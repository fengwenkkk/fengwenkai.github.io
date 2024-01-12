#include "g_include.h"

#ifdef USE_GOS_UNITTEST
TEST_CASE("gos_atou�ļ򵥲���", "[gos][gos_atou]")
{
    SECTION("�������")
    {
        SECTION("�������,����")
        {
            unsigned ulResult = 0;
            char acNum[] = "123";
            REQUIRE(gos_atou(acNum, &ulResult));
            REQUIRE(ulResult==123);
        }
        SECTION("�������, ��")
        {
            unsigned ulResult = 123;
            char acNum[] = "0";
            REQUIRE(gos_atou(acNum, &ulResult));
            REQUIRE(ulResult==0);
        }
        SECTION("�������,���unsignedֵ")
        {
            unsigned ulResult = 0;
            char acNum[32] = "";
            snprintf(acNum, sizeof(acNum)/sizeof(acNum[0]), "%u", UINT_MAX);
            REQUIRE(gos_atou(acNum, &ulResult));
            REQUIRE(ulResult==UINT_MAX);
        }
        SECTION("�������,��Сunsignedֵ")
        {
            unsigned ulResult = 123;
            char acNum[32] = "";
            snprintf(acNum, sizeof(acNum)/sizeof(acNum[0]), "%u", 0);
            REQUIRE(gos_atou(acNum, &ulResult));
            REQUIRE(ulResult==0);
        }
        SECTION("�������, ��һ���ַ�Ϊ�ո�������ʶ��")
        {
            unsigned ulResult = 0;
            REQUIRE(gos_atou(" 123", &ulResult));
            REQUIRE(123 == ulResult);
            /// ���unsignedֵ, �޷���������
            char acNum[32] = "";
            snprintf(acNum, sizeof(acNum)/sizeof(acNum[0]), " %u", UINT_MAX);
            REQUIRE(gos_atou(acNum, &ulResult));

            /// ��Сunsignedֵ
            snprintf(acNum, sizeof(acNum)/sizeof(acNum[0]), " %d", 0);
            REQUIRE(gos_atou(acNum, &ulResult));
            REQUIRE(ulResult==0);
        }
        SECTION("��������� ��һ���ַ���+���ܹ�����ʶ��")
        {
            unsigned ulResult = 0;
            REQUIRE(gos_atou("+123", &ulResult));
            REQUIRE(ulResult == 123);
        }
        SECTION("�ַ�������ǰ����������")
        {
            unsigned ulResult = 0;
            REQUIRE(gos_atou("0123", &ulResult));
            REQUIRE(ulResult == 123);
            REQUIRE(gos_atou("0000000000000456", &ulResult));
            REQUIRE(ulResult == 456);
        }
        SECTION("�ַ���ǰ����ֿո�����")
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

    SECTION("�쳣���")
    {
        SECTION("����")
        {
            unsigned ulResult = 0;
            char acNum[] = "-123";
            REQUIRE(!gos_atou(acNum, &ulResult));
            /// REQUIRE(ulResult==-123);
        }

        SECTION("��ֵ��� UINT_MAX + 1")
        {
            unsigned ulResult = 0;
            char acNum[32] = "";
            snprintf(acNum, sizeof(acNum)/sizeof(acNum[0]), "%llu", (unsigned long long)UINT_MAX + 1);
            REQUIRE(!gos_atou(acNum, &ulResult));
            /// REQUIRE(ulResult == UINT_MAX + 1);
        }
        SECTION("��ֵ��� -1")
        {
            unsigned ulResult = 0;
            char acNum[32] = "";
            snprintf(acNum, sizeof(acNum)/sizeof(acNum[0]), "%llu", (unsigned long long)0 - 1);
            REQUIRE(!gos_atou(acNum, &ulResult));
            /// REQUIRE(ulResult == 0 - 1);
        }
        SECTION("��ָ��")
        {
            unsigned ulResult = 0;
            REQUIRE(!gos_atou(NULL, &ulResult));
            REQUIRE(!gos_atou("123", NULL));
            REQUIRE(!gos_atou(NULL, NULL));
        }
        SECTION("���ַ���")
        {
            unsigned ulResult = 0;
            REQUIRE(!gos_atou("", &ulResult));
        }
        SECTION("�ַ����쳣, һ�������ո� ���쳣����һ������0")
        {
            unsigned ulResult = 123;
            REQUIRE(gos_atou(" ", &ulResult));
            REQUIRE(ulResult == 0);
            ulResult = 123;
            REQUIRE(gos_atou("   ", &ulResult));
            REQUIRE(ulResult == 0);
        }
        SECTION("�ַ����쳣�� �Ƿ��ַ�")
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
