#include "g_include.h"

#ifdef USE_GOS_UNITTEST
TEST_CASE("gos_atof�ļ򵥲���", "[lee][gos_atof]")
{
    SECTION("�������")
    {
        SECTION("��������")
        {
            double dTmp;
            char ac[] = "123.456";
            REQUIRE(gos_atof(ac, &dTmp));
            REQUIRE(Approx(123.456).margin(0.00001) == dTmp);
        }
        SECTION("��������")
        {
            double dTmp;
            char ac[] = "-123.456";
            REQUIRE(gos_atof(ac, &dTmp));
            REQUIRE(Approx(-123.456).margin(0.00001) == dTmp);
        }
        SECTION("��")
        {
            double dTmp;
            char ac[] = "0";
            REQUIRE(gos_atof(ac, &dTmp));
            REQUIRE(Approx(0).margin(0.00001) == dTmp);
            dTmp = 0.12345;
            char ac1[] = "0.0";
            REQUIRE(gos_atof(ac1, &dTmp));
            REQUIRE(Approx(0).margin(0.00001) == dTmp);
            dTmp = 0.12345;
            char ac2[] = "0.0000000";
            REQUIRE(gos_atof(ac2, &dTmp));
            REQUIRE(Approx(0).margin(0.00001) == dTmp);
        }
        SECTION("ֻ����������")
        {
            double dTmp;
            char ac[] = "123";
            REQUIRE(gos_atof(ac, &dTmp));
            REQUIRE(Approx(123).margin(0.00001) == dTmp);
            dTmp = 345;
            char ac1[] = "123.0";
            REQUIRE(gos_atof(ac1, &dTmp));
            REQUIRE(Approx(123).margin(0.00001) == dTmp);
            dTmp = 1;
            char ac2[] = "-123";
            REQUIRE(gos_atof(ac2, &dTmp));
            REQUIRE(Approx(-123).margin(0.00001) == dTmp);
            dTmp = 345;
            char ac3[] = "-123.0";
            REQUIRE(gos_atof(ac3, &dTmp));
            REQUIRE(Approx(-123).margin(0.00001) == dTmp);
        }
        SECTION("ֻ��С������")
        {
            double dTmp;
            char ac[] = "0.123";
            REQUIRE(gos_atof(ac, &dTmp));
            REQUIRE(Approx(0.123).margin(0.00001) == dTmp);
            dTmp = 345;
            char ac1[] = "0.12300";
            REQUIRE(gos_atof(ac1, &dTmp));
            REQUIRE(Approx(0.123).margin(0.00001) == dTmp);
            dTmp = 1;
            char ac2[] = "-0.123";
            REQUIRE(gos_atof(ac2, &dTmp));
            REQUIRE(Approx(-0.123).margin(0.00001) == dTmp);
            dTmp = 345;
            char ac3[] = "-0.123000000";
            REQUIRE(gos_atof(ac3, &dTmp));
            REQUIRE(Approx(-0.123).margin(0.00001) == dTmp);
        }
    }
}
#endif
