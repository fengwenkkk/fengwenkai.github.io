
#include "g_include.h"

#ifdef USE_GOS_UNITTEST

TEST_CASE("uuid µÄ¼òµ¥²âÊÔ", "[gos][uuid]")
{
    std::set<std::string> set;
    for(UINT32 i=  0; i < 10000; i++)
    {
        REQUIRE(set.insert(gos::uuid::GetUUID()).second);
    }
}

#endif