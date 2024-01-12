#include "g_include.h"
#include <stdint.h>
#include <limits.h>

#ifdef USE_GOS_UNITTEST
TEST_CASE("gos_atoi�ļ򵥲���", "[gos][gos_atoi]")
{
    SECTION("�������")
    {
        SECTION("�������,����")
        {
            int iResult = 0;
            char acNum[] = "123";
            REQUIRE(gos_atoi(acNum, &iResult));
            REQUIRE(iResult==123);
        }
        SECTION("�������, ��")
        {
            int iResult = -1;
            char acNum[] = "0";
            REQUIRE(gos_atoi(acNum, &iResult));
            REQUIRE(iResult==0);
        }
        SECTION("�������,����")
        {
            int iResult = 0;
            char acNum[] = "-123";
            REQUIRE(gos_atoi(acNum, &iResult));
            REQUIRE(iResult==-123);
        }
        SECTION("�������,���intֵ")
        {
            int iResult = 0;
            char acNum[32] = "";
            snprintf(acNum, sizeof(acNum)/sizeof(acNum[0]), "%d", INT_MAX);
            REQUIRE(gos_atoi(acNum, &iResult));
            REQUIRE(iResult==INT_MAX);
        }
        SECTION("�������,��Сintֵ")
        {
            int iResult = 0;
            char acNum[32] = "";
            snprintf(acNum, sizeof(acNum)/sizeof(acNum[0]), "%d", INT_MIN);
            REQUIRE(gos_atoi(acNum, &iResult));
            REQUIRE(iResult==INT_MIN);
        }
        SECTION("�������, ��һ���ַ�Ϊ�ո�������ʶ��")
        {
            int iResult = 0;
            char ac[] = "123";
            REQUIRE(gos_atoi(ac, &iResult));
            REQUIRE(123 == iResult);
            /// ���intֵ
            char acNum[32] = "";
            snprintf(acNum, sizeof(acNum)/sizeof(acNum[0]), " %d", INT_MAX);
            REQUIRE(gos_atoi(acNum, &iResult));
            REQUIRE(iResult==INT_MAX);
            /// ��Сintֵ
            snprintf(acNum, sizeof(acNum)/sizeof(acNum[0]), " %d", INT_MIN);
            REQUIRE(gos_atoi(acNum, &iResult));
            REQUIRE(iResult==INT_MIN);
        }
        SECTION("��������� ��һ���ַ���+���ܹ�����ʶ��")
        {
            int iResult = 0;
            char ac[] = "123";
            REQUIRE(gos_atoi(ac, &iResult));
            REQUIRE(iResult == 123);
        }
        SECTION("�ַ�������ǰ����������")
        {
            int iResult = 0;
            char ac0[] = "0123";
            REQUIRE(gos_atoi(ac0, &iResult));
            REQUIRE(iResult == 123);
            char ac[] = "0000000000000456";
            REQUIRE(gos_atoi(ac, &iResult));
            REQUIRE(iResult == 456);
        }
        SECTION("�ַ���ǰ����ֿո�����")
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

    SECTION("�쳣���")
    {
        SECTION("��ֵ��� INT_MAX + 1, �����һ��������ֵ���޷���������")
        {
            int iResult = 0;
            char acNum[32] = "";
            snprintf(acNum, sizeof(acNum)/sizeof(acNum[0]), "%lld", (long long)INT_MAX + 1);
            REQUIRE(gos_atoi(acNum, &iResult));
            REQUIRE(iResult == INT_MIN);
        }
        SECTION("��ֵ��� INT_MIN - 1, �����һ��������ֵ���޷���������")
        {
            int iResult = 0;
            char acNum[32] = "";
            snprintf(acNum, sizeof(acNum)/sizeof(acNum[0]), "%lld", (long long)INT_MIN - 1);
            REQUIRE(gos_atoi(acNum, &iResult));
            REQUIRE(iResult == INT_MAX);
        }
        SECTION("��ָ��")
        {
            int iResult = 0;
            char ac[] = "123";
            REQUIRE(!gos_atoi(NULL, &iResult));
            REQUIRE(!gos_atoi(ac, NULL));
            REQUIRE(!gos_atoi(NULL, NULL));
        }
        SECTION("���ַ���")
        {
            int iResult = 0;
            char ac[5] = "";
            REQUIRE(!gos_atoi(ac, &iResult));
        }
        SECTION("�ַ����쳣, һ�������ո� ���쳣����һ������0")
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
        SECTION("�ַ����쳣�� �Ƿ��ַ�")
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
