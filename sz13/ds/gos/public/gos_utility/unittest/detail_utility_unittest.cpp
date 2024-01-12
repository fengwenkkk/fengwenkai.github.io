#include "g_include.h"

#ifdef USE_GOS_UNITTEST
TEST_CASE("gos::to_string 简单测试 ", "[gos][to_string]")
{
    SECTION("入参为long long")
    {
        long long i64Num = 1;
        REQUIRE("1" == gos::to_string(i64Num));
    }
}
#endif
