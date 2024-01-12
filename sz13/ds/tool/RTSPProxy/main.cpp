//TODO List
/*
0、一直连接不上RTSP Server则失败（最多尝试30次）
1、超时未收到RTSP Server的RTP报文需要结束（30秒）
2、网关是否需要周期性向RTSPServer发送心跳报文（30秒一次）
3、客户端可能超时未发送OPTIONS等心跳报文（ffplay等过一段时间后将不再发送）
4、URL不可达时，RTSPServer返回404 Not Found，需要退出（最多尝试30次）
*/

#include "g_include.h"
#include "rtsp_util.h"
#include "rtsp_client.h"
#include "rtsp_server.h"
#include "rtp.h"

#ifndef _OSWIN32_
///#include "../libgos/func/g_conf_file.cpp"
#include "../libgos/func/g_lock.cpp"
#include "../libgos/func/g_logger.cpp"
#include "../libgos/func/g_log.cpp"
#include "../libgos/func/g_string.cpp"
#include "../libgos/queue/g_hash.cpp"
#include "../libgos/queue/g_hashlist.cpp"
#include "../libgos/queue/g_list.cpp"
#include "../libgos/func/g_proc.cpp"
#include "../libgos/func/g_json.cpp"
#include "../libgos/comm/gos_ping.c"
#include "rtsp_util.cpp"
#include "rtsp_client.cpp"
#include "rtsp_server.cpp"
#include "rtp.cpp"
#endif

UINT32 dtp_get_fd_num(FDSET* fds)
{
#ifdef _OSWIN32_
    return fds->fd_count;

#else
    UINT32  i;
    UINT32  ulCount = 0;

    for (i = 0; i < FD_SETSIZE; i++)
    {
        if (FD_ISSET(i, fds))
        {
            ulCount++;
        }
    }

    return ulCount;
#endif
}

/**
 * @brief           获取fdset中最大的fd
 * @param           fds    [in] 需要检测的fd集合
 * @return          UINT32 返回fdset中最大的fd
 * @author          zhengwenjie(zhengwenjie@gbcom.com.cn)
 * @par 示例:
 * @code
 *  
 * @endcode
 * @note
 */
static UINT32 dtp_get_max_fd(FDSET *fds)
{
#ifdef _OSWIN32_
    UINT32  i;
    SOCKET  sock = 0;

    for (i=0; i<fds->fd_count; i++)
    {
        if (i == 0 || sock < fds->fd_array[i])
        {
            sock = fds->fd_array[i];
        }
    }

    return sock;
#else
    INT32   i;

    for (i=FD_SETSIZE-1; i>=0; i--)
    {
        if (FD_ISSET(i, fds))
        {
            return i;
        }
    }

    return 0;
#endif
}

/*******************************************************************************
* 函数名称: dtp_get_socket_by_fds
* 功    能: 获取socket
* 参    数:
* 参数名称          输入/输出       描述
* fds               IN              文件描述符集合
* ulIndex           IN              索引
* 函数返回: SOCKET文件描述符
* 说    明:
*******************************************************************************/
static SOCKET dtp_get_socket_by_fds(FDSET *fds, UINT32 ulIndex)
{
#ifdef _OSWIN32_
    if (ulIndex >= fds->fd_count)
    {
        return INVALID_SOCKET;
    }

    return fds->fd_array[ulIndex];
#else
    if (FD_ISSET(ulIndex, fds))
    {
        return ulIndex;
    }

    return INVALID_SOCKET;
#endif
}

/**
 * @brief           初始化日志模块
 * @return          BOOL 
 * @author          zhengwenjie(zhengwenjie@gbcom.com.cn)
 * @par 示例:
 * @code
 *  
 * @endcode
 * @note
 * 1、2MB 2个
 */
BOOL SysInit(CHAR *szInst, UINT32 ulLogLevel)
{
    CHAR        acLogDir[256];
    Logger      *pLogger = GLogFactory::GetInstance()->GetDefaultLogger();

    pLogger->SetLogToFile(TRUE, 5, 6*1024*1024, 0);
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

        pLogger->SetLogToFile(TRUE, 2, 2*1024*1024, 0);
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
#else
    pLogger->SetLogToStdout(TRUE);
    pLogger->SetPrintColor(TRUE);
#endif

    if (!IS_NULL_STRING(szInst))
    {
        pLogger->SetLogFilePostfix(szInst);
//        GosLog(LOG_INFO, "log postfix %s", szInst);
    }

    GosLog(LOG_FATAL, "Sys init successful!");

    return TRUE;
}

/**
 * @brief           打印有效fd
 * @param           fds    [in] 需要检测打印的fd集合
 * @return          VOID 
 * @author          zhengwenjie(zhengwenjie@gbcom.com.cn)
 * @par 示例:
 * @code
 *  
 * @endcode
 * @note
 */
VOID PrintFDSet(FD_SET *fds)
{
    for (UINT32 i=0; i<FD_SETSIZE; i++)
    {
        SOCKET  stSocket = dtp_get_socket_by_fds(fds, i);
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
    FD_SET  except_fds;
    TIMEVAL stTimeout = {0};
//    UINT32  ulRet;
    UINT32  i;
    INT32   iRet;
    INT32   iNoDelay = 1;
    INT32   iErrCode = 0;
    INT32   iRecvLen;   
    SOCKET  stMaxSock;
    SOCKET  stClientSocket;
    BOOL    bNeedClose = FALSE;
    SOCKADDR_IN stAddr;
    INT32       iAddrLen = sizeof(SOCKADDR);
    UINT32      ulFdCount = 0;
    UINT32      ulTotalFdCount;
    UINT32      ulRTSPClientCloseTime = gos_get_sys_uptime();     // 最后一次收到RTSP客户端消息时间

    FD_ZERO(&g_fdsAll);
    FD_SET(g_stLocalRTSPServerSock, &g_fdsAll);

again:
    stTimeout.tv_sec = ulTimeout/1000;
    stTimeout.tv_usec = 1000*MOD(ulTimeout,1000);
    fds = g_fdsAll;
    except_fds = fds;

    CheckRTSPClientTimeout();

    if (ulRTSPClientCloseTime > 0 &&
        (gos_get_sys_uptime() - ulRTSPClientCloseTime) > g_ulRTSPClientConnectTimeout)
    {
        GosLog(LOG_INFO, "wait rtsp client to connect timeout for %u seconds", gos_get_sys_uptime() - ulRTSPClientCloseTime);
        CloseApp();
    }

    stMaxSock = dtp_get_max_fd(&fds);
    iRet = select(stMaxSock + 1, &fds, NULL, &except_fds, &stTimeout);
    if (iRet == 0)
    {
        goto again;
    }

    if (iRet < 0)
    {
        if (errno == EINTR)
        {
            GosLog(LOG_ERROR, "select return is EINTR");
            goto again;
        }

        GosLog(LOG_ERROR, "recv msg failed, %s", gos_get_socket_err());

        PrintFDSet(&except_fds);

        return -1;
    }

    if (FD_ISSET(g_stLocalRTSPServerSock, &fds))
    {
        FD_CLR(g_stLocalRTSPServerSock, &fds);
        stClientSocket = accept (g_stLocalRTSPServerSock, (SOCKADDR*)&stAddr, (socklen_t*)&iAddrLen);
        if (stClientSocket == INVALID_SOCKET)
        {
            GosLog(LOG_ERROR, "accept new connect failed, %s", gos_get_socket_err());
            goto again;
        }

        stAddr.sin_port = ntohs(stAddr.sin_port);

        /* 设置NoDelay方式 */
        setsockopt(stClientSocket, IPPROTO_TCP, TCP_NODELAY, (CHAR*)&iNoDelay, sizeof(iNoDelay));

        GosLog(LOG_INFO, "accept client socket(%u) of " IP_FMT ":%u ok", stClientSocket,
                IP_ARG(&stAddr.sin_addr.s_addr), (UINT32)stAddr.sin_port);

        NewConnInfo(stClientSocket, stAddr);
        ulRTSPClientCloseTime = 0; // 0表示已经连接，不再检测

        goto again;
    }

    // RTSP client -> Local
    if (g_stRTSPClientConnInfo.stLocalRTPSocket != INVALID_SOCKET &&
        FD_ISSET(g_stRTSPClientConnInfo.stLocalRTPSocket, &fds))
    {
        iRet = RecvRTPMsg(&g_stRTSPClientConnInfo);

        goto again;
    }

    // RTSP client -> Local
    if (g_stRTSPClientConnInfo.stLocalRTCPSocket != INVALID_SOCKET &&
    	FD_ISSET(g_stRTSPClientConnInfo.stLocalRTCPSocket, &fds))
    {
        iRet = RecvRTCPMsg(&g_stRTSPClientConnInfo);
        GosLog(LOG_INFO, "recv rtcp msg %d", iRet);

        goto again;
    }

    // RTSP Server -> Local
    if (g_stRTSPClientConnInfo.stLocalSocket != INVALID_SOCKET &&
    	FD_ISSET(g_stRTSPClientConnInfo.stLocalSocket, &fds))
    {
        iRet = RecvRTSPServerMsg(&g_stRTSPClientConnInfo, &iErrCode);

        if (iRet != 0 && iErrCode != 0)
        {
            GosLog(LOG_INFO, "recv rtsp server msg %d, err code %d", iRet, iErrCode);            
        }
        else
        {
            if (iErrCode == 0)
            {
                GosLog(LOG_DETAIL, "recv rtsp server msg %d", iRet);   
            }
            else
            {
                GosLog(LOG_DETAIL, "recv rtsp server msg %d, err code %d", iRet, iErrCode);   
            }
        }

        if (iRet < 0 || iErrCode < 0)
        {
            CloseApp();
        }

        if (iRet == 0 && iErrCode == 0)
        {
            gos_sleep_1ms(1000);    // 避免出现频繁打印
            //CloseApp();
            GosLog(LOG_INFO, "close local rtsp client socket %u", g_stRTSPClientConnInfo.stLocalSocket); 
            FD_CLR(g_stRTSPClientConnInfo.stLocalSocket, &g_fdsAll);
            CLOSE_SOCKET(g_stRTSPClientConnInfo.stLocalSocket);

            g_ulConnectRTSPServerSuccTime = 0;

            // 如果RTSP Server可达，但URL错误，则对方会主动断开连接
            FD_CLR(g_stRTSPClientConnInfo.stClientSocket, &g_fdsAll);
            CLOSE_SOCKET(g_stRTSPClientConnInfo.stClientSocket);

            ulRTSPClientCloseTime = gos_get_sys_uptime();
        }

        goto again;
    }

    /* 处理 tcp 端口上的消息 */
    ulFdCount = 0;
    ulTotalFdCount = dtp_get_fd_num(&fds);
    for (i=0; i<FD_SETSIZE; i++)
    {
        if (ulFdCount >= ulTotalFdCount)
        {
            break;
        }

        stClientSocket = dtp_get_socket_by_fds(&fds, i);
        if (stClientSocket == INVALID_SOCKET)
        {
            continue;
        }

        if (stClientSocket == g_stLocalRTSPServerSock)
        {
            continue;
        }

        ulFdCount++;

        /* 有消息到, 进行处理 */
        iErrCode = 0;
        bNeedClose = FALSE;
        if (stClientSocket != g_stRTSPClientConnInfo.stClientSocket) // TODO 不用for循环
        {
            continue;
        }

        iRecvLen = RecvRTSPClientMsg(stClientSocket, &iErrCode);

        if (iRecvLen != 0 || iErrCode != 0)
        {
            GosLog(LOG_INFO, "socket %u recv return %d, err code %d", stClientSocket, iRecvLen, iErrCode);
        }

        if (iRecvLen < 0)
        {
            if (iErrCode != 0)
            {
                if (SECONNRESET == iErrCode)
                {
                    GosLog(LOG_WARN, "server_listen_msg: client socket(%d) closed due to connect reset",
                        stClientSocket);
                }
                else
                {
                    GosLog(LOG_WARN, "server_listen_msg: client socket(%d) closed due to %s",
                        stClientSocket, gos_get_socket_err());
                }

                bNeedClose = TRUE;
            }
        }
        else if (iRecvLen == 0)
        {
            bNeedClose = TRUE;
        }

        if (bNeedClose)
        {
            GosLog(LOG_ERROR, "close client socket %u", stClientSocket);
          //  DelConnInfo(&g_stRTSPClientConnInfo);
            FD_CLR(stClientSocket, &g_fdsAll);
            CLOSE_SOCKET(stClientSocket); 

            g_stRTSPClientConnInfo.stClientSocket = INVALID_SOCKET;

            ulRTSPClientCloseTime = gos_get_sys_uptime();
        }
    }

    goto again;
}

// rtsp proxy是单线程程序
// 命令参数：-iInstantID -sRTSPServerAddr[:port] -lLocalRTSPServerAddr[:port] -d -cxxx -tyyy 
int main(int argc, char **argv)
{
    UINT32  ulLogLevel = LOG_INFO;
    UINT32  ulTimeout;
    CHAR    *szRTSPServerAddr = NULL;
    CHAR    *szLocalRTSPServerAddr = NULL;
    CHAR    *szInst = (CHAR*)"";

    // 获取日志等级及进程实例编号
    for (int i=1; i<argc; i++)
    {
        if (gos_str_like(argv[i], "-i"))    // 进程实例号
        {
            szInst = argv[i]+2;     
        }
        else if (strcmp(argv[i], "-d") == 0)    // detail打印
        {
            ulLogLevel = LOG_DETAIL;
        }
        else if (gos_str_like(argv[i], "-t"))    // 客户端心跳超时时间
        {
            if (gos_atou(argv[i]+2, &ulTimeout))
            {
                g_ulRTSPClientTimeout = ulTimeout;
                if (0 == g_ulRTSPClientTimeout)
                {
                    g_ulRTSPClientTimeout = 0xffffffff;
                }
            }
        }
        else if (gos_str_like(argv[i], "-c"))    // 客户端连接（重连）超时时间
        {
            if (gos_atou(argv[i]+2, &ulTimeout))
            {
                g_ulRTSPClientConnectTimeout = ulTimeout;
                if (g_ulRTSPClientConnectTimeout < 10)
                {
                    g_ulRTSPClientConnectTimeout = 10;
                }
            }
        }
    }

    gos_init_socket();
    SysInit(szInst, ulLogLevel);

    for (int i=1; i<argc; i++)
    {
        if (gos_str_like(argv[i], "-s"))
        {
            szRTSPServerAddr = argv[i];
            if (!gos_get_ip_port(argv[i]+2, g_aucRTSPServerAddr, &g_usRTSPServerPort))
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

            if (!gos_get_ip_port(argv[i]+2, g_aucLocalRTSPServerAddr, &g_usLocalRTSPServerPort))
            {
                GosLog(LOG_ERROR, "invalid param %s", argv[i]);
                return -1;
            }

            if (g_usLocalRTSPServerPort == 0)
            {
                g_usLocalRTSPServerPort = g_usDefaultRTSPServerPort;
            }
        }
    }

    if (!szRTSPServerAddr || !szLocalRTSPServerAddr)
    {
        GosLog(LOG_INFO, "invalid rtsp proxy parameters");
        return -1;
    }

    GosLog(LOG_INFO, "start rtsp proxy: %s %s", szRTSPServerAddr, szLocalRTSPServerAddr);

    g_stLocalRTSPServerSock = InitLocalRTSPServerSocket(g_aucLocalRTSPServerAddr, g_usLocalRTSPServerPort);
    if (g_stLocalRTSPServerSock == INVALID_SOCKET)
    {
        return -1;
    }

    GosLog(LOG_INFO, "create local rtsp server socket %u", g_stLocalRTSPServerSock);

    while(1)
    {
        RecvMsg();
//        gos_sleep_1ms(1);
    }

    return 0;
}

