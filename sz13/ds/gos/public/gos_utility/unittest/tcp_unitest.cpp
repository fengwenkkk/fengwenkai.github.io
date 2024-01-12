#include "g_include.h"

#ifdef USE_GOS_UNITTEST

constexpr int TCP_TEST_PORT = 62002;

static std::vector<gos::CallbackParam> g_vecTestRecv;  ///< 接收缓冲区
gos::mutex m_vecTestRecvMutex;

static void test_callback_recv(gos::CallbackParam sp)
{
    gos::lock_guard<gos::mutex> Guard(m_vecTestRecvMutex);
    g_vecTestRecv.push_back(sp);
}

static std::vector<gos::CallbackParam> test_callback_get()
{
    gos::lock_guard<gos::mutex> Guard(m_vecTestRecvMutex);
    return g_vecTestRecv;
}

static void test_clear()
{
    gos::lock_guard<gos::mutex> Guard(m_vecTestRecvMutex);
    g_vecTestRecv.clear();
}

TEST_CASE("gos::tcp_server gos::tcp_client 测试", "[gos][tcp]")
{
    PROFILER();///< 这个 profiler 的调用是为了初始化 profiler 的logger, 排除 Profiler创建的logger影响下面的内存测试
    gos::memory_check_impl checker;
    checker.start();
    {
        ///PROFILER();
        TIMEVAL stTimeOut;
        stTimeOut.tv_sec = 1;
        gos::tcp_server server(TCP_TEST_PORT, test_callback_recv, stTimeOut);
        server.Start();

        IPV4_INFO_C stIp;
        stIp.strIP = "127.0.0.1";
        stIp.iPort = TCP_TEST_PORT;
        gos::tcp_client<unsigned char> client(stIp);

        std::vector<unsigned char> vecSend(1024);
        gos::iota(vecSend, 0);
        REQUIRE(test_callback_get().empty());
        REQUIRE(client.Send(vecSend));
        gos_sleep_1ms(2);
        std::vector<gos::CallbackParam> vecRead = test_callback_get();
        REQUIRE(!vecRead.empty());
        REQUIRE(*vecRead.at(0) == vecSend);

        test_clear();
    }
    bool IsNotLeak = checker.finish();
    CHECK(IsNotLeak);      ///< lijiancong(2022-01-27): 这里的内存泄露是因为
                           ///< client.Send()->gos_tcp_send()->send() 函数引起的内存变化，暂时没有搞清楚
}

#endif