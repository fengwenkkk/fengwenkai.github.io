#include "g_include.h"

#ifdef USE_GOS_UNITTEST

constexpr int TCP_TEST_PORT = 62002;

static std::vector<gos::CallbackParam> g_vecTestRecv;  ///< ���ջ�����
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

TEST_CASE("gos::tcp_server gos::tcp_client ����", "[gos][tcp]")
{
    PROFILER();///< ��� profiler �ĵ�����Ϊ�˳�ʼ�� profiler ��logger, �ų� Profiler������loggerӰ��������ڴ����
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
    CHECK(IsNotLeak);      ///< lijiancong(2022-01-27): ������ڴ�й¶����Ϊ
                           ///< client.Send()->gos_tcp_send()->send() ����������ڴ�仯����ʱû�и����
}

#endif