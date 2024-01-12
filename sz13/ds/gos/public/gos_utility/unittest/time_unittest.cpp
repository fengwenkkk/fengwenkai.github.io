
#include "g_include.h"

#ifdef USE_GOS_UNITTEST
TEST_CASE("tick_count ºÚµ•≤‚ ‘", "[gos][tick_count]")
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

    SECTION("–›√ﬂ 1s ≤‚ ‘")
    {
        gos::tick_count timer;
        timer.start();
        gos_sleep(1);
        timer.finish();
        REQUIRE(gos::approx(1000, 20).equal(timer.get_ms()));
    }
}

TEST_CASE("sleep_ms ºÚµ•≤‚ ‘", "[gos][sleep_ms]")
{
    SECTION("–›√ﬂ 1ms ≤‚ ‘")
    {
        gos::tick_count timer;
        timer.start();
        gos::sleep_ms(1);
        timer.finish();
        REQUIRE(gos::approx(1, 1).equal(timer.get_ms()));
    }

    SECTION("–›√ﬂ 5ms ≤‚ ‘")
    {
        gos::tick_count timer;
        timer.start();
        gos::sleep_ms(5);
        timer.finish();
        REQUIRE(gos::approx(5, 1).equal(timer.get_ms()));
    }

    SECTION("–›√ﬂ 16ms ≤‚ ‘")
    {
        gos::tick_count timer;
        timer.start();
        gos::sleep_ms(16);
        timer.finish();
        REQUIRE(gos::approx(16, 1).equal(timer.get_ms()));
    }

    SECTION("–›√ﬂ 1s ≤‚ ‘")
    {
        gos::tick_count timer;
        timer.start();
        gos::sleep_ms(1000);
        timer.finish();
        REQUIRE(gos::approx(1000, 1).equal(timer.get_ms()));
    }
}
#endif