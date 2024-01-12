#include "g_include.h"

#ifdef USE_GOS_UNITTEST
TEST_CASE("gos::memory_check_impl 堆变量测试 ", "[gos][memory_check_impl]")
{
    SECTION("未泄漏情况0")
    {
        gos::memory_check_impl ss;
        ss.start();
        /// DoNothing
        bool iRet = ss.finish();  ///< 不能直接使用 REQUIRE() 包裹该函数，会必现 finish 函数返回false
        REQUIRE(iRet);
    }

    SECTION("未泄漏情况1")
    {
        gos::memory_check_impl ss;
        ss.start();
        int* p = new int(5);
        delete p;
        bool iRet = ss.finish();  ///< 不能直接使用 REQUIRE() 包裹该函数，会必现 finish 函数返回false
        REQUIRE(iRet);
    }

    SECTION("泄漏情况")
    {
        gos::memory_check_impl ss;
        ss.start();
        int* p = new int(5);
        bool iRet = ss.finish();  ///< 不能直接使用 REQUIRE() 包裹该函数，会必现 finish 函数返回false
        REQUIRE(!iRet);
        delete p;
    }
}

static int* memory_check_impl_test_leak()
{
    return new int;
}

static void memory_check_impl_test_not_leak()
{
    int i;
    (void)i;
}

TEST_CASE("gos::memory_check_impl 栈变量测试 ", "[gos][memory_check_impl]")
{
    SECTION("未泄漏情况: 变量")
    {
        gos::memory_check_impl ss;
        ss.start();
        int p = 5;
        bool iRet = ss.finish();  ///< 不能直接使用 REQUIRE() 包裹该函数，会必现 finish 函数返回false
        REQUIRE(iRet);
        (void)p;  ///< 防止编译器优化
    }

    SECTION("未泄漏情况1: 变量")
    {
        gos::memory_check_impl ss;
        ss.start();
        {
            int p = 5;
            bool iRet = ss.finish();  ///< 不能直接使用 REQUIRE() 包裹该函数，会必现 finish 函数返回false
            REQUIRE(iRet);
            (void)p;  ///< 防止编译器优化
        }
    }

    SECTION("泄漏情况: 函数")
    {
        gos::memory_check_impl ss;
        ss.start();
        int* p = memory_check_impl_test_leak();
        bool iRet = ss.finish();  ///< 不能直接使用 REQUIRE() 包裹该函数，会必现 finish 函数返回false
        REQUIRE(!iRet);
        delete p;
    }

    SECTION("未泄漏情况: 函数")
    {
        gos::memory_check_impl ss;
        ss.start();
        memory_check_impl_test_not_leak();
        bool iRet = ss.finish();  ///< 不能直接使用 REQUIRE() 包裹该函数，会必现 finish 函数返回false
        REQUIRE(iRet);
    }
}

TEST_CASE("gos::memory_check 简单测试 ", "[gos][memory_check]")
{
    /// 本测试用例因为结果都在析构函数中，所以需要配合断点测试
    SECTION("未泄漏的情况0")
    {
        {
            gos::memory_check obj("", "", 0);
            obj.start();
        }
    }

    SECTION("未泄漏的情况1")
    {
        {
            gos::memory_check obj("", "", 0);
            obj.start();
            int i = 9;
            (void)i;  ///< 防止编译器优化
        }
    }

    SECTION("未泄漏的情况2")
    {
        int* p = nullptr;
        {
            gos::memory_check obj("", "", 0);
            obj.start();
            p = new int;
            delete p;
        }
    }

    SECTION("未泄漏的情况： const char* ")
    {
        {
            MEMORY_CHECK();
            const char* sz = "Hello World!";
            (void)sz;
        }
    }

    /// 这里应该配合断点测试
    SECTION("泄漏的情况")
    {
        int* p = nullptr;
        {
            gos::memory_check obj("", "", 0);
            obj.start();
            p = new int;
        }

        delete p;
    }
}

TEST_CASE("MEMORY_CHECK 简单测试 ", "[gos][MEMORY_CHECK]")
{
    /// 本测试用例因为结果都在析构函数中，所以需要配合断点测试
    SECTION("未泄漏的情况： DoNothing")
    {
        {
            MEMORY_CHECK();
        }
    }

    SECTION("未泄漏的情况: 临时变量")
    {
        {
            MEMORY_CHECK();
            int i = 9;
            (void)i;  ///< 防止编译器优化
        }
    }

    SECTION("未泄漏的情况: new")
    {
        int* p = nullptr;
        {
            MEMORY_CHECK();
            p = new int;
            delete p;
        }
    }

    SECTION("未泄漏的情况： const char* ")
    {
        {
            MEMORY_CHECK();
            const char* sz = "Hello World!";
            (void)sz;
        }
    }

    /// 这里应该配合断点测试
    SECTION("泄漏的情况")
    {
        int* p = nullptr;
        {
            MEMORY_CHECK();
            p = new int;
        }

        delete p;
    }
}

#endif