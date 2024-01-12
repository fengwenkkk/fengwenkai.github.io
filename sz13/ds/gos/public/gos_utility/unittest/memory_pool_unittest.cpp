#include "g_include.h"

#ifdef USE_GOS_UNITTEST
TEST_CASE("第三方内存池 简单测试 ", "[gos][thirdparty][MemoryPool]")
{
    SECTION("allocate deallocate")
    {
        gos::memory_check_impl ss;
        ss.start();
        {
            MemoryPool<size_t> pool;
            size_t* x = pool.allocate();

            *x = 0xDEADBEEF;

            pool.deallocate(x);
        }
        bool iRet = ss.finish();
        REQUIRE(iRet);
    }

    SECTION("newElement deleteElement")
    {
        gos::memory_check_impl ss;
        ss.start();
        {
            MemoryPool<size_t> pool;
            size_t* x = pool.newElement(0);

            *x = 0xDEADBEEF;
            std::cout << std::hex << *x << std::endl;

            pool.deleteElement(x);
        }
        bool iRet = ss.finish();
        REQUIRE(iRet);
    }
}

TEST_CASE("第三方内存池 分配器测试 ", "[gos][thirdparty][MemoryPool]")
{
    SECTION("通过传入 vector<int> 来对比内存池的分配速度")
    {
        int a[64 * 1024];
        gos::iota(a, a + 64 * 1024, 0);
        gos::ProfilerImpl ProfilerNormal;
        ProfilerNormal.Start();
        std::vector<int> vecNormal(a, a + 64 * 1024);
        (void)vecNormal;
        ProfilerNormal.Finish();

        gos::ProfilerImpl ProfilerMemoryPool;
        ProfilerMemoryPool.Start();
        std::vector<int> vecMemoryPool(a, a + 64 * 1024);
        (void)vecMemoryPool;
        ProfilerMemoryPool.Finish();

        /// std::cout << std::dec << "Normal: " << ProfilerNormal.GetTimeDiffMicrosecond() << " us\n";
        /// std::cout << std::dec << "Memory pool: " << ProfilerMemoryPool.GetTimeDiffMicrosecond() << " us\n";
        /// Normal: 153 us
        /// Memory pool: 128 us

        /// 验证 MemoryPool 版本的 vector 比默认分配器的 vector 构造速度快
        REQUIRE(ProfilerNormal.GetTimeDiffMicrosecond() > ProfilerMemoryPool.GetTimeDiffMicrosecond());
    }

    SECTION("通过传入 vector<unsigned char> 来对比内存池的分配速度")
    {
        unsigned char auc[64 * 1024];
        gos::iota(auc, auc + 64 * 1024, 0);
        gos::ProfilerImpl ProfilerNormal;
        ProfilerNormal.Start();
        std::vector<int> vecNormal(auc, auc + 64 * 1024);
        (void)vecNormal;
        ProfilerNormal.Finish();

        gos::ProfilerImpl ProfilerMemoryPool;
        ProfilerMemoryPool.Start();
        std::vector<int> vecMemoryPool(auc, auc + 64 * 1024);
        (void)vecMemoryPool;
        ProfilerMemoryPool.Finish();

        /// std::cout << std::dec << "Normal: " << ProfilerNormal.GetTimeDiffMicrosecond() << " us\n";
        /// std::cout << std::dec << "Memory pool: " << ProfilerMemoryPool.GetTimeDiffMicrosecond() << " us\n";
        /// Normal: 3867 us
        /// Memory pool: 3815 us

        /// 验证 MemoryPool 版本的 vector 比默认分配器的 vector 构造速度快
        REQUIRE(ProfilerNormal.GetTimeDiffMicrosecond() > ProfilerMemoryPool.GetTimeDiffMicrosecond());
    }
}

#endif