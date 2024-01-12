#include "g_include.h"

#ifdef USE_GOS_UNITTEST
TEST_CASE("gos::memory_check_impl �ѱ������� ", "[gos][memory_check_impl]")
{
    SECTION("δй©���0")
    {
        gos::memory_check_impl ss;
        ss.start();
        /// DoNothing
        bool iRet = ss.finish();  ///< ����ֱ��ʹ�� REQUIRE() �����ú���������� finish ��������false
        REQUIRE(iRet);
    }

    SECTION("δй©���1")
    {
        gos::memory_check_impl ss;
        ss.start();
        int* p = new int(5);
        delete p;
        bool iRet = ss.finish();  ///< ����ֱ��ʹ�� REQUIRE() �����ú���������� finish ��������false
        REQUIRE(iRet);
    }

    SECTION("й©���")
    {
        gos::memory_check_impl ss;
        ss.start();
        int* p = new int(5);
        bool iRet = ss.finish();  ///< ����ֱ��ʹ�� REQUIRE() �����ú���������� finish ��������false
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

TEST_CASE("gos::memory_check_impl ջ�������� ", "[gos][memory_check_impl]")
{
    SECTION("δй©���: ����")
    {
        gos::memory_check_impl ss;
        ss.start();
        int p = 5;
        bool iRet = ss.finish();  ///< ����ֱ��ʹ�� REQUIRE() �����ú���������� finish ��������false
        REQUIRE(iRet);
        (void)p;  ///< ��ֹ�������Ż�
    }

    SECTION("δй©���1: ����")
    {
        gos::memory_check_impl ss;
        ss.start();
        {
            int p = 5;
            bool iRet = ss.finish();  ///< ����ֱ��ʹ�� REQUIRE() �����ú���������� finish ��������false
            REQUIRE(iRet);
            (void)p;  ///< ��ֹ�������Ż�
        }
    }

    SECTION("й©���: ����")
    {
        gos::memory_check_impl ss;
        ss.start();
        int* p = memory_check_impl_test_leak();
        bool iRet = ss.finish();  ///< ����ֱ��ʹ�� REQUIRE() �����ú���������� finish ��������false
        REQUIRE(!iRet);
        delete p;
    }

    SECTION("δй©���: ����")
    {
        gos::memory_check_impl ss;
        ss.start();
        memory_check_impl_test_not_leak();
        bool iRet = ss.finish();  ///< ����ֱ��ʹ�� REQUIRE() �����ú���������� finish ��������false
        REQUIRE(iRet);
    }
}

TEST_CASE("gos::memory_check �򵥲��� ", "[gos][memory_check]")
{
    /// ������������Ϊ����������������У�������Ҫ��϶ϵ����
    SECTION("δй©�����0")
    {
        {
            gos::memory_check obj("", "", 0);
            obj.start();
        }
    }

    SECTION("δй©�����1")
    {
        {
            gos::memory_check obj("", "", 0);
            obj.start();
            int i = 9;
            (void)i;  ///< ��ֹ�������Ż�
        }
    }

    SECTION("δй©�����2")
    {
        int* p = nullptr;
        {
            gos::memory_check obj("", "", 0);
            obj.start();
            p = new int;
            delete p;
        }
    }

    SECTION("δй©������� const char* ")
    {
        {
            MEMORY_CHECK();
            const char* sz = "Hello World!";
            (void)sz;
        }
    }

    /// ����Ӧ����϶ϵ����
    SECTION("й©�����")
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

TEST_CASE("MEMORY_CHECK �򵥲��� ", "[gos][MEMORY_CHECK]")
{
    /// ������������Ϊ����������������У�������Ҫ��϶ϵ����
    SECTION("δй©������� DoNothing")
    {
        {
            MEMORY_CHECK();
        }
    }

    SECTION("δй©�����: ��ʱ����")
    {
        {
            MEMORY_CHECK();
            int i = 9;
            (void)i;  ///< ��ֹ�������Ż�
        }
    }

    SECTION("δй©�����: new")
    {
        int* p = nullptr;
        {
            MEMORY_CHECK();
            p = new int;
            delete p;
        }
    }

    SECTION("δй©������� const char* ")
    {
        {
            MEMORY_CHECK();
            const char* sz = "Hello World!";
            (void)sz;
        }
    }

    /// ����Ӧ����϶ϵ����
    SECTION("й©�����")
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