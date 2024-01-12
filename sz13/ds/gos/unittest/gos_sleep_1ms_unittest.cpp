#include "g_include.h"

#ifdef USE_GOS_UNITTEST
TEST_CASE("gos_sleep_1ms ºÚµ•≤‚ ‘", "[gos][gos_sleep_1ms]")
{
    SECTION("–›√ﬂ 1ms ≤‚ ‘")
    {
        gos::tick_count timer;
        timer.start();
        gos_sleep_1ms(1);
        timer.finish();
        REQUIRE(gos::approx(1, 20).equal(timer.get_ms()));
    }

    SECTION("–›√ﬂ 5ms ≤‚ ‘")
    {
        gos::tick_count timer;
        timer.start();
        gos_sleep_1ms(5);
        timer.finish();
        REQUIRE(gos::approx(5, 20).equal(timer.get_ms()));
    }

    SECTION("–›√ﬂ 16ms ≤‚ ‘")
    {
        gos::tick_count timer;
        timer.start();
        gos_sleep_1ms(16);
        timer.finish();
        REQUIRE(gos::approx(16, 20).equal(timer.get_ms()));
    }

    SECTION("–›√ﬂ 1000ms ≤‚ ‘")
    {
        gos::tick_count timer;
        timer.start();
        gos_sleep_1ms(1000);
        timer.finish();
        REQUIRE(gos::approx(1000, 20).equal(timer.get_ms()));
    }
}

#endif