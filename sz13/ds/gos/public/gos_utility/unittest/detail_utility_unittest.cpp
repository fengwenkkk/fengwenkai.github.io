#include "g_include.h"

#ifdef USE_GOS_UNITTEST
TEST_CASE("gos::to_string �򵥲��� ", "[gos][to_string]")
{
    SECTION("���Ϊlong long")
    {
        long long i64Num = 1;
        REQUIRE("1" == gos::to_string(i64Num));
    }
}
#endif
