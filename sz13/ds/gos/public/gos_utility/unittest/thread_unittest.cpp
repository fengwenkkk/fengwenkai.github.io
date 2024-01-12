
#include "g_include.h"

#ifdef USE_GOS_UNITTEST

class test_thread : public GThread
{
public:
    test_thread() : GThread(nullptr) {}
    virtual GOS_THREAD_RET ThreadEntry(void *pPara)
    {
        while (IsThreadRunning())
        {
            gos_sleep_1ms(1);
        }
        return 0;
    };
};

TEST_CASE("Gthread ºÚµ•≤‚ ‘", "[gos][GThread]")
{
    SECTION("Join ≤‚ ‘ ƒ⁄¥ÊºÏ≤È")
    {
        for (int i = 0; i < 100; i++)
        {
            gos::memory_check_impl checker;
            checker.start();
            {
                test_thread thread;
                thread.Start();
                thread.Stop();
            }
            bool IsNotLeak = checker.finish();
            REQUIRE(IsNotLeak);
        }
    }

    SECTION("Join ≤‚ ‘ ƒ⁄¥ÊºÏ≤È ¥¯—”≥Ÿ")
    {
        for (int i = 0; i < 10; i++)
        {
            gos::memory_check_impl checker;
            checker.start();
            {
                test_thread thread;
                thread.Start();
                gos_sleep_1ms(2);
                thread.Stop();
            }
            bool IsNotLeak = checker.finish();
            REQUIRE(IsNotLeak);
        }
    }

    SECTION("IsThreadEnd ≤‚ ‘ ƒ⁄¥ÊºÏ≤È")
    {
        for (int i = 0; i < 100; i++)
        {
            gos::memory_check_impl checker;
            checker.start();
            {
                gos::shared_ptr<test_thread> thread(new test_thread);
                thread->Start();
                thread->Stop();
            }
            bool IsNotLeak = checker.finish();
            REQUIRE(IsNotLeak);
        }
    }
}

class test_thread_1 : gos::noncopyable, public GThread
{
public:
    test_thread_1() : GThread(nullptr) {}
    virtual GOS_THREAD_RET ThreadEntry(void *pPara)
    {
        while (IsThreadRunning())
        {
            gos_sleep_1ms(1);
        }
        return 0;
    };
};

TEST_CASE("Gthread1 ºÚµ•≤‚ ‘", "[gos][GThread]")
{
    SECTION("Join ≤‚ ‘ ƒ⁄¥ÊºÏ≤È")
    {
        for (int i = 0; i < 100; i++)
        {
            gos::memory_check_impl checker;
            checker.start();
            {
                test_thread_1 thread;
                thread.Start();
                thread.Stop();
            }
            bool IsNotLeak = checker.finish();
            REQUIRE(IsNotLeak);
        }
    }

    SECTION("Join ≤‚ ‘ ƒ⁄¥ÊºÏ≤È ¥¯—”≥Ÿ")
    {
        for (int i = 0; i < 10; i++)
        {
            gos::memory_check_impl checker;
            checker.start();
            {
                test_thread_1 thread;
                thread.Start();
                gos_sleep_1ms(2);
                thread.Stop();
            }
            bool IsNotLeak = checker.finish();
            REQUIRE(IsNotLeak);
        }
    }

    SECTION("IsThreadEnd ≤‚ ‘ ƒ⁄¥ÊºÏ≤È")
    {
        for (int i = 0; i < 100; i++)
        {
            gos::memory_check_impl checker;
            checker.start();
            {
                gos::shared_ptr<test_thread_1> thread(new test_thread_1);
                thread->Start();
                thread->Stop();
            }
            bool IsNotLeak = checker.finish();
            REQUIRE(IsNotLeak);
        }
    }
}

#endif