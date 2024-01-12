#include "g_include.h"

#ifdef USE_GOS_UNITTEST
TEST_CASE("ProfilerImpl �򵥲���", "[gos][profiler]")
{
    SECTION("�򵥲���")
    {
        gos::ProfilerImpl profiler;
        profiler.Start();
        profiler.Finish();
        REQUIRE(gos::approx(0, 20).equal(profiler.GetTimeDiffMillisecond()));
    }

    SECTION("�߳����� 1ms ����")
    {
        gos::ProfilerImpl profiler;
        profiler.Start();
        gos_sleep_1ms(1);
        profiler.Finish();
        REQUIRE(gos::approx(1, 20).equal(profiler.GetTimeDiffMillisecond()));
    }

    SECTION("�߳����� 5ms ����")
    {
        gos::ProfilerImpl profiler;
        profiler.Start();
        gos_sleep_1ms(5);
        profiler.Finish();
        REQUIRE(gos::approx(5, 20).equal(profiler.GetTimeDiffMillisecond()));
    }

    SECTION("�߳����� 1000ms ����")
    {
        gos::ProfilerImpl profiler;
        profiler.Start();
        gos_sleep_1ms(1000);
        profiler.Finish();
        REQUIRE(gos::approx(1000, 20).equal(profiler.GetTimeDiffMillisecond()));
    }
}

#endif