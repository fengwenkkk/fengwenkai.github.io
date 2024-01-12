#include "g_include.h"

#ifdef USE_GOS_UNITTEST
TEST_CASE("GString�� ���ܲ��� ", "[gos][GString]")
{
    char ac[1024] = {0};
    gos::iota(ac, ac + sizeof(ac), 0);

    SECTION("GString: 10, Append(1024): 100")
    {
        gos::memory_check_impl ss;
        ss.start();
        PROFILER_INFO("GString�� ���ܲ��� GString: 10, Append(1024): 100");
        for (unsigned x = 0; x < 10; ++x)
        {
            GString str;
            for (unsigned i = 0; i < 100; ++i)
            {
                str.Append(ac);
            }
        }
        bool iRet = ss.finish();  ///< ����ֱ��ʹ�� REQUIRE() �����ú���������� finish ��������false
        REQUIRE(!iRet);
    }

    SECTION("GString: 10, Append(1024): 1k")
    {
        gos::memory_check_impl ss;
        ss.start();
        PROFILER_INFO("GString�� ���ܲ��� GString: 10, Append(1024): 1k");
        for (unsigned x = 0; x < 10; ++x)
        {
            GString str;
            for (unsigned i = 0; i < 1000; ++i)
            {
                str.Append(ac);
            }
        }
        bool iRet = ss.finish();  ///< ����ֱ��ʹ�� REQUIRE() �����ú���������� finish ��������false
        REQUIRE(!iRet);
    }

    SECTION("GString: 10, Append(1024): 10k")
    {
        gos::memory_check_impl ss;
        ss.start();
        PROFILER_INFO("GString�� ���ܲ��� GString: 10, Append(1024): 10k");
        for (unsigned x = 0; x < 10; ++x)
        {
            GString str;
            for (unsigned i = 0; i < 10000; ++i)
            {
                str.Append(ac);
            }
        }
        bool iRet = ss.finish();  ///< ����ֱ��ʹ�� REQUIRE() �����ú���������� finish ��������false
        REQUIRE(!iRet);
    }

    SECTION("GString: 10, Append(1024): 100k")
    {
        gos::memory_check_impl ss;
        ss.start();
        PROFILER_INFO("GString�� ���ܲ��� GString: 10, Append(1024): 100k");
        for (unsigned x = 0; x < 10; ++x)
        {
            GString str;
            for (unsigned i = 0; i < 100000; ++i)
            {
                str.Append(ac);
            }
        }
        bool iRet = ss.finish();  ///< ����ֱ��ʹ�� REQUIRE() �����ú���������� finish ��������false
        REQUIRE(!iRet);
    }
}

#endif