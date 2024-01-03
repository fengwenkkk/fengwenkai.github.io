// rtspproxy.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "g_include.h"
#include "rtp.h"
#include "rtsp_util.h"
#include "rtsp_server.h"


//启动日志打印
BOOL SysInit(CHAR* szInst, UINT32 ulLogLevel)
{
    CHAR        acLogDir[256];
    Logger* pLogger = GLogFactory::GetInstance()->GetDefaultLogger();
    pLogger->SetLogToFile(TRUE, 5, 6 * 1024 * 1024, 0);
    pLogger->SetLogLevel(ulLogLevel);
#ifdef _OSWIN32_
    sprintf(acLogDir, "%s/log", gos_get_root_path());
    if (!gos_file_exist(acLogDir))
    {
        if (!gos_create_dir(acLogDir))
        {
            GosLog(LOG_FATAL, "create dir %s failed", acLogDir);
            return FALSE;
        }
    }
#elif _LINUX_
    sprintf(acLogDir, "%s/log", gos_get_root_path());
    if (!gos_file_exist(acLogDir))
    {
        if (!gos_create_dir(acLogDir))
        {
            GosLog(LOG_FATAL, "create dir %s failed", acLogDir);
            return FALSE;
        }
    }
#else
    sprintf(acLogDir, "/mnt/emmc/logsave");
    if (!gos_file_exist(acLogDir))
    {
        sprintf(acLogDir, "/tmp/log");
        if (!gos_file_exist(acLogDir))
        {
            if (!gos_create_dir(acLogDir))
            {
                GosLog(LOG_FATAL, "create dir %s failed", acLogDir);
                return FALSE;
            }
        }
        pLogger->SetLogToFile(TRUE, 2, 2 * 1024 * 1024, 0);
        pLogger->SetLogFilePath(acLogDir);
        GosLog(LOG_ERROR, "/mnt/emmc/logsave does not exist");
    }
    else
    {
        pLogger->SetLogFilePath(acLogDir);
    }
#endif
#ifndef _OSWIN32_
    pLogger->SetLogToStdout(FALSE);
#elif _LINUX_
    pLogger->SetLogToStdout(FALSE);
#else
    pLogger->SetLogToStdout(TRUE);
    pLogger->SetPrintColor(TRUE);
#endif
    if (!IS_NULL_STRING(szInst))
    {
        pLogger->SetLogFilePostfix(szInst);
    }
    GosLog(LOG_FATAL, "Sys init successful!");
    return TRUE;
}

//打印有效套接字
VOID PrintFDSet(FD_SET* fds)
{
    for (UINT32 i = 0; i < FD_SETSIZE; i++)
    {
        SOCKET  stSocket = gos_get_socket_by_fds(fds, i);
        if (stSocket != INVALID_SOCKET)
        {
            GosLog(LOG_INFO, "socket %d = %d", i, stSocket);
        }
    }
}

INT32 RecvMsg()
{
    UINT32  ulTimeout = 1000;
    FD_SET  fds;
    FD_SET  except_fds;//异常情况监测
    TIMEVAL stTimeout = { 0 };

    INT32   iRet;//select返回值
    INT32   iNoBlockingFlag = 1;
    INT32   iRecvLen;
    SOCKET  stClientSocket;
    SOCKET  stMaxSock;
    SOCKADDR_IN stAddr;
    INT32       iAddrLen = sizeof(SOCKADDR);

    UINT32      ulRTSPClientCloseTime = gos_get_sys_uptime();

    FD_ZERO(&g_fdsAll);
    FD_SET(g_stLocalRTSPServerSock, &g_fdsAll);

again:
    stTimeout.tv_sec = ulTimeout / 1000;
    stTimeout.tv_usec = 1000 * MOD(ulTimeout, 1000);
    fds = g_fdsAll;
    except_fds = fds;


    //检查RTSP客户端的超时情况。如果超时时间已经超过设定的阈值，则关闭应用程序。
    if (ulRTSPClientCloseTime > 0 &&
        (gos_get_sys_uptime() - ulRTSPClientCloseTime) > g_ulRTSPClientConnectTimeout)
    {
        GosLog(LOG_INFO, "wait rtsp client to connect timeout for %u seconds", gos_get_sys_uptime() - ulRTSPClientCloseTime);
        CloseApp();
    }

    stMaxSock = gos_get_max_fd(&fds);
    
    //调用select函数来监视文件描述符的状态变化，包括是否可读或是否出现异常。如果没有事件发生，则再次进行循环。
    iRet = select(stMaxSock + 1, &fds, NULL, &except_fds, &stTimeout);
    if (iRet == 0)
    {
        goto again;
    }
    if (iRet < 0)
    {
        if (errno == EINTR)
        {
            GosLog(LOG_ERROR, "Select return error: EINTR");
            goto again;
        }
        GosLog(LOG_ERROR, "Recv msg failed, error:%s", gos_get_socket_err());
        PrintFDSet(&except_fds);
        return -1;
    }

    //有事件发生
    if (FD_ISSET(g_stLocalRTSPServerSock, &fds))
    {
        FD_CLR(g_stLocalRTSPServerSock, &fds);
        stClientSocket = accept(g_stLocalRTSPServerSock, (SOCKADDR*)&stAddr, (socklen_t*)&iAddrLen);
        if (stClientSocket == INVALID_SOCKET)
        {
            GosLog(LOG_ERROR, "Accept new connect failed,error: %s", gos_get_socket_err());
            goto again;
        }
        stAddr.sin_port = ntohs(stAddr.sin_port);

        //设置为不阻塞
        //在网络编程中，TCP_NODELAY 是一个套接字选项常量，用于控制 TCP 连接的延迟和性能。通常情况下，TCP 协议使用 Nagle 算法将小的数据块合并成更大的数据块，以提高网络效率。但在某些特定场景下，例如实时通信或传输实时数据流等，需要尽快发送数据而不考虑合并的效果。
        setsockopt(stClientSocket, IPPROTO_TCP, TCP_NODELAY, (CHAR*)&iNoBlockingFlag, sizeof(iNoBlockingFlag));
        GosLog(LOG_INFO, "Accept client socket(%u) of " IP_FMT ":%u ok", stClientSocket, IP_ARG(&stAddr.sin_addr.s_addr), (UINT32)stAddr.sin_port);
        NewConnInfo(stClientSocket, stAddr);
        ulRTSPClientCloseTime = 0;

    }

    //接收RTP数据,发给本地服务器
    if (g_stRTSPClientConnInfo.stLocalRTPSocket != INVALID_SOCKET &&
        FD_ISSET(g_stRTSPClientConnInfo.stLocalRTPSocket, &fds))
    {
        iRet = RecvRTPMsg(&g_stRTSPClientConnInfo);
        goto again;
    }

     //接收RTCP数据,发给本地服务器
    if (FD_ISSET(g_stRTSPClientConnInfo.stLocalRTCPSocket, &fds))
    {
        iRet = RecvRTCPMsg(&g_stRTSPClientConnInfo);
       

        goto again;
    }
     
     //接收RTSP数据

    



}





//输入参数 -sRTSPServerAddr[:port] -lLocalRTSPServerAddr[:port]
int main(int argc, char** argv)
{
    UINT32  ulLogLevel = LOG_INFO;
    UINT32  ulTimeout;//超时时间
    CHAR* szRTSPServerAddr = NULL;//服务端
    CHAR* szLocalRTSPServerAddr = NULL;//本地
    CHAR* szInst = (CHAR*)"";

    //超时时间设置
    for (int i = 1; i < argc; i++)
    {
        if (gos_str_like(argv[i], "-i"))
        {
            szInst = argv[i] + 2;
        }
        else if (strcmp(argv[i], "-d") == 0)
        {
            ulLogLevel = LOG_DETAIL;
        }
       
        else if (gos_str_like(argv[i], "-t"))
        {
            if (gos_atou(argv[i] + 2, &ulTimeout))
            {
                g_ulRTSPClientTimeout = ulTimeout;
                if (0 == g_ulRTSPClientTimeout)
                {
                    g_ulRTSPClientTimeout = 0xffffffff;
                }
            }
        }
        else if (gos_str_like(argv[i], "-c"))
        {
            if (gos_atou(argv[i] + 2, &ulTimeout))
            {
                g_ulRTSPClientConnectTimeout = ulTimeout;
                if (g_ulRTSPClientConnectTimeout < 10)
                {
                    g_ulRTSPClientConnectTimeout = 10;
                }
            }
        }
    }
    
    //启动环境和日志
    gos_init_socket();
    SysInit(szInst, ulLogLevel);

    //检查命令行输入-s[服务地址]:[端口] -l[本地地址]:[端口]
    for (int i = 1; i < argc; i++)
    {
        if (gos_str_like(argv[i], "-s"))
        {
            szRTSPServerAddr = argv[i];
            if (!gos_get_ip_port(argv[i] + 2, g_aucRTSPServerAddr, &g_usRTSPServerPort))//固定RTSP端口554
            {
                GosLog(LOG_ERROR, "invalid param %s", argv[i]);
                return -1;
            }
            if (g_usRTSPServerPort == 0)
            {
                g_usRTSPServerPort = g_usDefaultRTSPServerPort;
            }
        }
        else if (gos_str_like(argv[i], "-l"))
        {
            szLocalRTSPServerAddr = argv[i];
            if (!gos_get_ip_port(argv[i] + 2, g_aucLocalRTSPServerAddr, &g_usLocalRTSPServerPort))
            {
                GosLog(LOG_ERROR, "invalid param %s", argv[i]);
                return -1;
            }
            if (g_usLocalRTSPServerPort == 0)
            {
                g_usLocalRTSPServerPort = g_usDefaultRTSPServerPort;//本地和服务一个端口？？
            }
        }
    }
    if (!szRTSPServerAddr || !szLocalRTSPServerAddr)
    {
        GosLog(LOG_INFO, "invalid rtsp proxy parameters");
        return -1;
    }
    GosLog(LOG_INFO, "Start rtsp proxy: %s : %s", szRTSPServerAddr, szLocalRTSPServerAddr);

    //循环接收
    while (1)
    {
        RecvMsg();
    }

    return 0;

}

