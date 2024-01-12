#include "g_include.h"

#ifdef USE_GOS_UNITTEST
TEST_CASE("gos::circular_queue ¼òµ¥²âÊÔ", "[gos][circular_queue]")
{
    SECTION("»ù´¡ÓÃÀý")
    {
        gos::circular_queue<int> q(3);
        REQUIRE(q.empty());
        REQUIRE(!q.full());
        q.push_back(1);
        q.push_back(2);
        REQUIRE(!q.full());
        q.push_back(3);
        REQUIRE(q.full());
        q.push_back(4);
        REQUIRE(q.full());
        REQUIRE(q.overrun_counter() == 1);

        REQUIRE(q.front() == 2);
        q.pop_front();
        REQUIRE(!q.full());
        REQUIRE(q.front() == 3);
        q.pop_front();
        REQUIRE(q.front() == 4);
        q.pop_front();
        REQUIRE(q.empty());
        REQUIRE(q.overrun_counter() == 1);
    }
}

#endif
