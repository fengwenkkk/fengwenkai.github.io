#include "g_include.h"
#include "ds_public.h"
#include "msg.h"
#include "DISPublic.h"
#include "ThreadGPS.h"

#define GPSLog(ulLogLevel, szFormat, ...)      { GosLog("GPS", ulLogLevel, szFormat,##__VA_ARGS__); }

ThreadGPS       *g_ThreadGPS  = NULL;


BOOL StartThreadGPS(UINT16 usPort)
{
    g_ThreadGPS = new ThreadGPS(usPort);

    g_ThreadGPS->Start();
    GPSLog(LOG_FATAL, "thread GPS started");

    return TRUE;
}

ThreadGPS::ThreadGPS(UINT16 usPort):GThread(NULL)
{
    m_usPort = usPort;
    m_ulLastSendCheckTime = 0;
    m_ulLastRecvCheckTime = 0;

    m_bRunning = TRUE;

    m_stSock = INVALID_SOCKET;
}

ThreadGPS::~ThreadGPS()
{
    CLOSE_SOCKET(m_stSock);
}

BOOL ThreadGPS::Init()
{
    if (m_stSock != INVALID_SOCKET)
    {
        return TRUE;
    }

    UINT8   aucLocalAddr[4] = {0,0,0,0};

    m_stSock = gos_create_udp_server_socket(aucLocalAddr, m_usPort);
    if (m_stSock == INVALID_SOCKET)
    {
        GPSLog(LOG_ERROR, "create udp socket on port %u failed", m_usPort);
        return FALSE;
    }

    GPSLog(LOG_INFO, "thread GPS listen on udp port %u", m_usPort);
    return TRUE;
}

VOID ThreadGPS::Stop()
{
    m_bRunning = FALSE;
}

INT32 ThreadGPS::RecvMsg()
{
    UINT32      ulTimeout = 1000;
    SOCKADDR_IN stClientAddr;

    memset(m_aucMsg, 0, sizeof(m_aucMsg));
    return gos_recv_udp_msg(m_stSock, m_aucMsg, sizeof(m_aucMsg)-1, ulTimeout, &stClientAddr);
}

GOS_THREAD_RET ThreadGPS::ThreadEntry(void* pPara)
{
    INT32       iRet;

    GPSLog(LOG_INFO, "thread GPS start running");

    while(m_bRunning)
    {
        if (!Init())
        {
            gos_sleep(3);
            continue;
        }

        iRet = RecvMsg();
        if (iRet <= 0)
        {
            continue;
        }

        OnRecvMsg((CHAR*)m_aucMsg);
    }

    CLOSE_SOCKET(m_stSock);

    return 0;
}

VOID ThreadGPS::OnRecvMsg(CHAR *szMsg)
{
    GPS_INFO_T  stGPS = {0};

    if (!ParseGPSInfo(szMsg, stGPS))
    {
        GPSLog(LOG_ERROR, "parse GPS info failed: %s", szMsg);
        return;
    }

    AddGPSInfo(stGPS);
}
