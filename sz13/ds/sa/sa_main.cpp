#include "sa_public.h"

#ifndef _OSWIN32_
#include "../libgos/func/g_conf_file.cpp"
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

#include "sa_public.cpp"
#include "sa_dis_msg.cpp"
#include "sa_iph_msg.cpp"
#include "sa_pa_msg.cpp"
#endif

//#define SZ14_MODE      "LINE_SZ14"
CHAR        g_acSAVersion[32] = {"v1.0.0.1"};


SA_CFG_T    g_stSACfg = {0};
SOCKET      g_stDISUDPClientSock = INVALID_SOCKET;
SOCKET      g_stPISTCPClientSock = INVALID_SOCKET;
SOCKET      g_stPATCPClientSock  = INVALID_SOCKET;
SOCKET      g_stPAUDPClientSock  = INVALID_SOCKET;
SOCKET      g_stIPHUDPClientSock  = INVALID_SOCKET;

SOCKADDR_IN g_stDISAddr = {0};
SOCKADDR_IN g_stPAUDPAddr = {0};

CHAR        g_acSAID[128] = {0};
UINT8       g_aucLocalAddr[4] = {0};
UINT8       g_aucGateway[4] = {0};

UINT32  g_ulTrainUnitID = 0xffffffff;
UINT32  g_ulMsgBufLen = MAX_MSG_BUF_SIZE;
UINT8   g_aucDISMsg[MAX_MSG_BUF_SIZE];
UINT8   g_aucPISMsg[MAX_MSG_BUF_SIZE];
UINT8   g_aucPAMsg[MAX_MSG_BUF_SIZE];
UINT32  g_ulCurrPISMsgLen = 0;
UINT32  g_ulCurrPAMsgLen = 0;

UINT32  g_ulDISSocketErrNum = 0;
///UINT32  g_ulPISSocketErrNum = 0;

///BOOL    g_bDiscoverPISServer = FALSE;
BOOL    g_bDiscoverPAServer = FALSE;
BOOL    g_bPISGetRecBrdCfg = FALSE;
BOOL    g_bSelfCheckSendStatus = TRUE;

static BOOL    g_bLogsavePathExist = TRUE;
UINT32  g_ulIPHLinkcheckMaxNum = 0;
UINT32  g_ulPALinkcheckMaxNum  = 0;

UINT32  g_ulIPHRunStatus;
UINT32  g_ulPAMasterStatus;
UINT32  g_ulPARunStatus;

BOOL SetLogLevel(UINT32 ulLogLevel)
{
    Logger          *pLogger = NULL;
    GLogFactory     *pLogFactory = GLogFactory::GetInstance();

    if (!pLogFactory)
    {
        return FALSE;
    }

    pLogger = pLogFactory->GetDefaultLogger();
    if (!pLogger)
    {
        return FALSE;
    }

    pLogger->SetLogLevel(ulLogLevel);

    return TRUE;
}

BOOL InitSysLog()
{
    Logger          *pLogger = NULL;
    GLogFactory     *pLogFactory = GLogFactory::GetInstance();
    UINT32          ulLogLevel = LOG_INFO;

    if (!pLogFactory)
    {
        GosLog(LOG_FATAL, "InitSysLog failed! In pLogFactory");
        return FALSE;
    }

    pLogger = pLogFactory->GetDefaultLogger();
    if (!pLogger)
    {
        GosLog(LOG_FATAL, "InitSysLog failed! In pLogger");
        return FALSE;
    }

#ifndef _OSWIN32_
    if (!g_bLogsavePathExist)
    {
        pLogger->SetLogFilePath((CHAR*)"/tmp/log");
    }
    else
    {
        pLogger->SetLogFilePath((CHAR*)"/mnt/emmc/logsave");
    }
#endif

    if (!CfgGet(CFG_SA_LOG_LEVEL, &ulLogLevel))
    {
        ulLogLevel = LOG_INFO;
        GosLog(LOG_INFO, "uci get log_level failed now default level is LOG_INFO!");
    }

    GosLog(LOG_INFO, "cfg value of %s is %u", CFG_SA_LOG_LEVEL, ulLogLevel);

#ifndef _OSWIN32_
    pLogger->SetLogToStdout(FALSE);
#else
    pLogger->SetLogToStdout(TRUE);
    pLogger->SetPrintColor(TRUE);
#endif
    if (!g_bLogsavePathExist)
    {
        pLogger->SetLogToFile(TRUE, 2, 2*1024*1024, 0);
    }
    else
    {
        pLogger->SetLogToFile(TRUE, 2, 60*1024*1024, 0);
    }

    pLogger->SetLogLevel(ulLogLevel);

    GosLog(LOG_FATAL, "InitSysLog successful!");

    return TRUE;
}

BOOL SysInit()
{
    CHAR        acDir[260];
    CHAR        acLogDir[256];

#ifdef _OSWIN32_
    sprintf(acDir, "%s/log", gos_get_root_path());
#else
    sprintf(acDir, "/tmp/log");
    sprintf(acLogDir, "/mnt/emmc/logsave");
#endif

    if (!gos_file_exist(acLogDir))
    {
        g_bLogsavePathExist = FALSE;
    }

    if (!gos_file_exist(acDir))
    {
        if (!gos_create_dir(acDir))
        {
            if (!g_bLogsavePathExist)
            {
                GosLog(LOG_FATAL, "create sa_log_dir failed! please check emmc!");
                return FALSE;
            }
        }
    }

    InitSysLog();

    GetSACfg();

    return TRUE;
}

BOOL GetLocalIP(UINT8 *pucServerIP, UINT32 ulTimeout, UINT8 *pucLocalIP)
{
    DOUBLE  dDelay = 0;
    UINT16  usSeq = 0;
    UINT32  ulTTL = 0;

    if (!gos_ping(pucServerIP, 64, ulTimeout, &dDelay, &usSeq, &ulTTL, pucLocalIP))
    {
        return FALSE;
    }

    return TRUE;
}

BOOL InitDISClientSocket()
{
    if (g_stDISUDPClientSock != INVALID_SOCKET)
    {
        return TRUE;
    }

    //g_ulCurrDISMsgLen = 0;
    g_stDISUDPClientSock = gos_create_udp_socket();
    if (g_stDISUDPClientSock == INVALID_SOCKET)
    {
        return FALSE;
    }

    gos_init_socket_addr(&g_stDISAddr, g_stSACfg.stDISAddr.aucIP, g_stSACfg.stDISAddr.usPort);

    GosLog(LOG_FATAL, "create DIS UDP client succ");

    return TRUE;
}

BOOL InitIPHUDPClientSocket()//UINT8 *pucAddr, SOCKET &stClientSock, SOCKADDR_IN &stAddr)
{
    if (g_stIPHUDPClientSock != INVALID_SOCKET)
    {
        return TRUE;
    }

    g_stIPHUDPClientSock = gos_create_udp_socket();
    if (g_stIPHUDPClientSock == INVALID_SOCKET)
    {
        return FALSE;
    }

    GosLog(LOG_FATAL, "create IPH UDP client succ");

    return TRUE;
}

BOOL InitPAUDPClientSocket()
{
    if (g_stPAUDPClientSock != INVALID_SOCKET)
    {
        return TRUE;
    }

    //g_ulCurrDISMsgLen = 0;
    g_stPAUDPClientSock = gos_create_udp_socket();
    if (g_stPAUDPClientSock == INVALID_SOCKET)
    {
        return FALSE;
    }

    gos_init_socket_addr(&g_stPAUDPAddr, g_stSACfg.stPAUDPAddr.aucIP, g_stSACfg.stPAUDPAddr.usPort);

    GosLog(LOG_FATAL, "create PA UDP client succ");

    return TRUE;
}

BOOL InitPATCPClientSocket()
{
    if (g_stPATCPClientSock != INVALID_SOCKET)
    {
        return TRUE;
    }

    GosLog(LOG_INFO, "connect to PA TCP server");
    g_stPATCPClientSock = gos_connect_tcp_server(g_stSACfg.stPATCPAddr.aucIP, g_stSACfg.stPATCPAddr.usPort);
    if (g_stPATCPClientSock == INVALID_SOCKET)
    {
        
        GosLog(LOG_ERROR, "connect to PA TCP server "IP_FMT"(%u) failed", IP_ARG(g_stSACfg.stPATCPAddr.aucIP), g_stSACfg.stPATCPAddr.usPort);

        return FALSE;
    }

    GosLog(LOG_FATAL, "connect to PA TCP  server "IP_FMT"(%u) succ", IP_ARG(g_stSACfg.stPATCPAddr.aucIP), g_stSACfg.stPATCPAddr.usPort);
    gos_sleep_1ms(5);

    return TRUE;
}

BOOL InitPISClientSocket()
{
    if (g_stPISTCPClientSock != INVALID_SOCKET)
    {
        return TRUE;
    }

/*    if (!gos_simple_ping(g_stSACfg.stPISAddr.aucIP, 100))
    {
        return FALSE;
    }
*/
    g_ulCurrPISMsgLen = 0;

    g_stPISTCPClientSock = gos_connect_tcp_server(g_stSACfg.stPISAddr.aucIP, g_stSACfg.stPISAddr.usPort);
    if (g_stPISTCPClientSock == INVALID_SOCKET)
    {
        //g_stSACfg.bAutoPISDiscover = FALSE;
        
        GosLog(LOG_ERROR, "connect to PIS server "IP_FMT"(%u) failed", IP_ARG(g_stSACfg.stPISAddr.aucIP), g_stSACfg.stPISAddr.usPort);

        return FALSE;
    }

    GosLog(LOG_FATAL, "connect to PIS server "IP_FMT"(%u) succ", IP_ARG(g_stSACfg.stPISAddr.aucIP), g_stSACfg.stPISAddr.usPort);

    //g_stSACfg.bAutoPISDiscover = TRUE;
    gos_sleep_1ms(5);

    return TRUE;
}

UINT32 RecvDISMsg()
{
    INT32   iRecvLen;
    UINT32  ulRecvLen = sizeof(g_aucDISMsg);
    DTP_MSG_HDR_T   *pstHdr = (DTP_MSG_HDR_T*)g_aucDISMsg;
    UINT32          ulMsgMagic;
    UINT16          usVer;

    iRecvLen = recv(g_stDISUDPClientSock, (CHAR*)g_aucDISMsg, ulRecvLen, 0);
    if (iRecvLen < 0)
    {
        GosLog(LOG_ERROR, "RecvDISMsg: recv DIS failed, %s", gos_get_socket_err());
        goto fail;
    }
    else if (iRecvLen == 0)
    {
        GosLog(LOG_ERROR, "RecvDISMsg: recv DIS failed, disconnect by peer");
        goto fail;
    }
    else if (iRecvLen < (int)sizeof(DTP_MSG_HDR_T))
    {
        GosLog(LOG_ERROR, "RecvDISMsg: recv DIS failed, length %d is too small", iRecvLen);
        goto fail;
    }

    ulMsgMagic = ntohl(pstHdr->ulMsgMagic);
    usVer = ntohs(pstHdr->usMsgVer);
    if (ulMsgMagic != DTP_MSG_MAGIC)
    {
        GosLog(LOG_ERROR, "RecvDISMsg: recv DIS failed, magic(%08X) is invalid", ulMsgMagic);
        goto fail;
    }

    if (usVer != DTP_MSG_VER)
    {
        GosLog(LOG_ERROR, "RecvDISMsg: recv DIS failed, ver(%02X) is invalid", usVer);
        goto fail;
    }

    ulRecvLen = ntohl(pstHdr->ulMsgLen);
    if ((INT32)ulRecvLen != iRecvLen)
    {
        GosLog(LOG_ERROR, "RecvDISMsg: recv DIS failed, recv length %d should be %u", iRecvLen, ulRecvLen);
        goto fail;
    }

    //g_ulCurrDISMsgLen = iRecvLen;
    return RECV_DIS_SUCC;

fail:
    return RECV_FAIL;
}

VOID InitPISSocket()
{
    CLOSE_SOCKET(g_stPISTCPClientSock);
    g_ulIPHLinkcheckMaxNum = 0;
}

VOID InitPASocket()
{
    CLOSE_SOCKET(g_stPATCPClientSock);
    g_ulPALinkcheckMaxNum = 0;
}

UINT32 RecvPISMsg()
{
    INT32   iRecvLen;
    UINT32  ulTotalRecvLen;
    UINT32  ulRecvLen;

    if (g_ulCurrPISMsgLen < sizeof(PIS_MSG_HDR_T))
    {
        ulRecvLen = sizeof(PIS_MSG_HDR_T) - g_ulCurrPISMsgLen;
        ulTotalRecvLen = sizeof(PIS_MSG_HDR_T);
    }
    else
    {
        PIS_MSG_HDR_T *pstHdr = (PIS_MSG_HDR_T*)g_aucPISMsg;

        ulTotalRecvLen = sizeof(PIS_MSG_HDR_T) + ntohs(pstHdr->usMsgLen);
        ulRecvLen = ulTotalRecvLen - g_ulCurrPISMsgLen;

        if (ulTotalRecvLen > MAX_MSG_BUF_SIZE)
        {
            GosLog(LOG_ERROR, "recvLen is too big : %u", ulTotalRecvLen);
            GosLog(LOG_ERROR, "pstHdr->usMsgLen is %#x", pstHdr->usMsgLen);

            return RECV_MSG_INVALID; /// goto fail;
        }
    }

    iRecvLen = recv(g_stPISTCPClientSock, (CHAR*)g_aucPISMsg + g_ulCurrPISMsgLen, ulRecvLen, 0);
    if (iRecvLen < 0)
    {
        GosLog(LOG_ERROR, "RecvPISMsg: recv PIS failed, %s", gos_get_socket_err());
        goto fail;
    }
    else if (iRecvLen == 0)
    {
        GosLog(LOG_ERROR, "RecvPISMsg: recv PIS failed, disconnect by peer");
        goto fail;
    }

    if (g_ulCurrPISMsgLen < sizeof(PIS_MSG_HDR_T))
    {
        PIS_MSG_HDR_T *pstHdr = (PIS_MSG_HDR_T*)g_aucPISMsg;

        g_ulCurrPISMsgLen += iRecvLen;
        if (g_ulCurrPISMsgLen < sizeof(PIS_MSG_HDR_T))
        {
            return RECV_NEEDMORE;
        }

        UINT16    usMsgMagic = NTOHS(pstHdr->usMsgMagic);

        if (usMsgMagic != PIS_MSG_MAGIC)
        {
            GosLog(LOG_ERROR, "RecvPISMsg: recv PIS failed, magic(%04X) is invalid", pstHdr->usMsgMagic);

            PrintMsg(pstHdr, g_ulCurrPISMsgLen);

            memset(g_aucPISMsg, 0, sizeof(g_aucPISMsg));
            return RECV_MSG_INVALID; ///goto fail;
        }

        ulRecvLen = ntohs(pstHdr->usMsgLen);
        if (ulRecvLen == 0)     // 没有消息体
        {
            g_ulCurrPISMsgLen = 0;
            return RECV_PIS_SUCC;
        }

        return RECV_NEEDMORE;
    }
    else
    {
        g_ulCurrPISMsgLen += iRecvLen;
        if (g_ulCurrPISMsgLen == ulTotalRecvLen)
        {
            g_ulCurrPISMsgLen = 0;
            return RECV_PIS_SUCC;
        }

        return RECV_NEEDMORE;
    }

fail:
    ///InitPISSocket();

    return RECV_FAIL;

}

UINT32 RecvPAMsg()
{
    INT32   iRecvLen;
    UINT32  ulTotalRecvLen;
    UINT32  ulRecvLen;

    if (g_ulCurrPAMsgLen < sizeof(PA_MSG_HDR_T))
    {
        ulRecvLen = sizeof(PA_MSG_HDR_T) - g_ulCurrPAMsgLen;
        ulTotalRecvLen = sizeof(PA_MSG_HDR_T);
    }
    else
    {
        PA_MSG_HDR_T *pstHdr = (PA_MSG_HDR_T*)g_aucPAMsg;

        ulTotalRecvLen = sizeof(PA_MSG_HDR_T) + ntohs(pstHdr->usMsgLen);
        ulRecvLen = ulTotalRecvLen - g_ulCurrPAMsgLen;

        if (ulTotalRecvLen > MAX_MSG_BUF_SIZE)
        {
            GosLog(LOG_ERROR, "recvLen is too big : %u", ulTotalRecvLen);
            GosLog(LOG_ERROR, "pstHdr->usMsgLen is %#x", pstHdr->usMsgLen);

            return RECV_MSG_INVALID; //goto fail;
        }
    }

    iRecvLen = recv(g_stPATCPClientSock, (CHAR*)g_aucPAMsg + g_ulCurrPAMsgLen, ulRecvLen, 0);
    if (iRecvLen < 0)
    {
        GosLog(LOG_ERROR, "RecvPAMsg: recv Pa failed, %s", gos_get_socket_err());
        goto fail;
    }
    else if (iRecvLen == 0)
    {
        GosLog(LOG_ERROR, "RecvPAMsg: recv PA failed, disconnect by peer");
        goto fail;
    }

    if (g_ulCurrPAMsgLen < sizeof(PA_MSG_HDR_T))
    {
        PA_MSG_HDR_T *pstHdr = (PA_MSG_HDR_T*)g_aucPAMsg;

        g_ulCurrPAMsgLen += iRecvLen;
        if (g_ulCurrPAMsgLen < sizeof(PA_MSG_HDR_T))
        {
            return RECV_NEEDMORE;
        }

        UINT16    usMsgMagic = NTOHS(pstHdr->usMsgMagic);

        if (usMsgMagic != PIS_MSG_MAGIC)
        {
            GosLog(LOG_ERROR, "RecvPAMsg: recv PA failed, magic(%02X) is invalid", pstHdr->usMsgMagic);

            PrintMsg(pstHdr, g_ulCurrPAMsgLen);
            memset(g_aucPAMsg, 0, sizeof(g_aucPAMsg));
            return RECV_MSG_INVALID; ///goto fail;
        }

        ulRecvLen = ntohs(pstHdr->usMsgLen);
        if (ulRecvLen == 0)     // 没有消息体
        {
            g_ulCurrPAMsgLen = 0;
            return RECV_PA_SUCC;
        }

        return RECV_NEEDMORE;
    }
    else
    {
        g_ulCurrPAMsgLen += iRecvLen;
        if (g_ulCurrPAMsgLen == ulTotalRecvLen)
        {
            g_ulCurrPAMsgLen = 0;
            return RECV_PA_SUCC;
        }

        GosLog(LOG_WARN, "RecvPAMsg: msg len is %u, total len should be", g_ulCurrPAMsgLen, ulTotalRecvLen);

        return RECV_NEEDMORE;
    }

fail:
   // InitPASocket();
    return RECV_FAIL;

}

UINT32 RecvSocketMsg(UINT32 ulTimeout)
{
    FD_SET  fds;
    FD_SET  fdsAll;
    TIMEVAL stTimeout = {0};
    UINT32  ulRet;
    INT32   iRet;
    INT32   iTimeout;
    UINT32  ulStartTime = gos_get_uptime_1ms();
    UINT32  ulTimePassed;
    SOCKET  stMaxSock = g_stDISUDPClientSock;

    if (g_stDISUDPClientSock == INVALID_SOCKET)
    {
        GosLog(LOG_ERROR, "invaild dis socket");
        return RECV_FAIL;
    }

    if (g_stPISTCPClientSock != INVALID_SOCKET)
    {
        stMaxSock = MAX(stMaxSock, g_stPISTCPClientSock);
    }

    if (g_stPATCPClientSock != INVALID_SOCKET)
    {
        stMaxSock = MAX(stMaxSock, g_stPATCPClientSock);
    }

    if (g_stPAUDPClientSock != INVALID_SOCKET)
    {
        stMaxSock = MAX(stMaxSock, g_stPAUDPClientSock);
    }

    FD_ZERO(&fdsAll);
    FD_SET(g_stDISUDPClientSock, &fdsAll);

    if (g_stPISTCPClientSock != INVALID_SOCKET)
    {
        FD_SET(g_stPISTCPClientSock, &fdsAll);
    }

    if (g_stPATCPClientSock != INVALID_SOCKET)
    {
        FD_SET(g_stPATCPClientSock, &fdsAll);
    }

again:
    ulTimePassed = gos_get_uptime_1ms() - ulStartTime;
    iTimeout = ulTimeout - ulTimePassed;

    if (iTimeout <= 0)
    {
        return RECV_TIMEOUT;
    }

    stTimeout.tv_sec = iTimeout/1000;
    stTimeout.tv_usec = 1000*MOD(iTimeout,1000);

    fds = fdsAll;

    iRet = select(stMaxSock + 1, &fds, NULL, NULL, &stTimeout);
    if (iRet == 0)
    {
        return RECV_TIMEOUT;
    }

    if (iRet < 0)
    {
        if (errno == EINTR)
        {
            goto again;
        }

        GosLog(LOG_ERROR, "RecvSocketMsg: recv failed, %s", gos_get_socket_err());
        CLOSE_SOCKET(g_stPISTCPClientSock);
        CLOSE_SOCKET(g_stPATCPClientSock);

        return RECV_FAIL;
    }

    if (FD_ISSET(g_stDISUDPClientSock, &fds))
    {
        ulRet = RecvDISMsg();
        if (ulRet == RECV_NEEDMORE || ulRet == RECV_TIMEOUT)
        {
            goto again;
        }

        return ulRet;
    }
    else if (g_stPISTCPClientSock != INVALID_SOCKET &&
             FD_ISSET(g_stPISTCPClientSock, &fds))
    {
        ulRet = RecvPISMsg();
        if (ulRet == RECV_NEEDMORE || ulRet == RECV_TIMEOUT)
        {
            goto again;
        }
        else if (ulRet == RECV_FAIL)
        {
            ///CLOSE_SOCKET(g_stPISTCPClientSock);
            InitPISSocket();
            return ulRet;
        }

        return ulRet;
    }
    else if (g_stPATCPClientSock != INVALID_SOCKET &&
             FD_ISSET(g_stPATCPClientSock, &fds))
    {
        ulRet = RecvPAMsg();
        if (ulRet == RECV_NEEDMORE || ulRet == RECV_TIMEOUT)
        {
            goto again;
        }
        else if (ulRet == RECV_FAIL)
        {
            ///CLOSE_SOCKET(g_stPATCPClientSock);
            InitPASocket();
            return ulRet;
        }

        return ulRet;
    }
    else
    {
        GosLog(LOG_ERROR, "invalid socket");
    }

    goto again;
}

VOID SetPISLinkStatus(BOOL bLinked)
{
    ram_set(APP_STATUS_PISLinkStatus, bLinked);
}

VOID SetPALinkStatus(BOOL bLinked)
{
    ram_set(APP_STATUS_PALinkStatus, bLinked);
}

BOOL InitPAServerSocket()
{
    if (!InitPATCPClientSocket())
    {
        GosLog(LOG_FATAL, "create PA TCP client fail");
        SetPALinkStatus(FALSE);
        return FALSE;
    }

    SetPALinkStatus(TRUE);

    if (!InitPAUDPClientSocket())
    {
        GosLog(LOG_FATAL, "create PA UDP client fail");
        return FALSE;
    }

    return TRUE;
}

BOOL InitIPHServerSocket()
{
    if (!InitPISClientSocket())
    {
        GosLog(LOG_FATAL, "create PIS TCP client fail");
        SetPISLinkStatus(FALSE);
        return FALSE;
    }
    
    SetPISLinkStatus(TRUE);

    if (!InitIPHUDPClientSocket())
    {
        GosLog(LOG_FATAL, "create IPH UDP client fail");
        return FALSE;
    }

    return TRUE;
}

BOOL ServiceAgentMain()
{
    UINT32  ulTimeout = 1000;
    UINT32  ulRecvRet;
    BOOL    bSetRtspClientAdrr = TRUE;

    static UINT32    ulLastCheckPAHeartBeatTime  = 0;
    static UINT32    ulLastCheckIPHHeartBeatTime = 0;

    if (!LoadConfig())
    {
        GosLog(LOG_ERROR, "load config failed");
        return FALSE;
    }

    sprintf(g_acSAID, IP_FMT, IP_ARG(g_aucLocalAddr));
    GosLog(LOG_INFO, "SA id is %s", g_acSAID);

    //SA 启动时对UCI作初始配置
    ram_set(TRAIN_COVER_STATUS, 1);
    ram_set(TRAIN_DRIVE_MODE, 1);
    SetPISLinkStatus(FALSE);
    SetPALinkStatus(FALSE);

    // 设置PIS服务器地址
    while(1)
    {
        CheckResetButton();

        if (bSetRtspClientAdrr)
        {
            if (g_stSACfg.bIPHVideoProxyMode != FALSE)
            {
                SetRtspLocalAddr();
                bSetRtspClientAdrr = FALSE;
            }
        }

        if (!InitDISClientSocket())
        {
            GosLog(LOG_ERROR, "Init dis client socket failed!");
            gos_sleep_1ms(10);
            continue;
        }

        SendDISGetCfgMsg();
        SendDISHeartBeatMsg();

        if (!gos_simple_ping(g_stSACfg.stPATCPAddr.aucIP, 10))
        {
            GosLog(LOG_ERROR, "Ping PA TCP server "IP_FMT" failed", IP_ARG(g_stSACfg.stPATCPAddr.aucIP));
            continue;
        }

        if (InitPAServerSocket())
        {
            SendPAHeartBeatMsg();
            SendPAGetTrainInfoMsg();
            
            ///超出三分钟不再继续发送自检结果
            ///1、	车载台每三秒钟上报一次自检结果
            ///2、	PA超过连续三次（9s）未收到自检上报认为自检失败
            ///3、	PA超过三分钟仍然未收到自检正常（0：正常，其他值：异常）认为自检失败
            ///4、	收到自检正常后PA忽略后续自检上报报文

            ///根据DIS中app_cfg配置表中的参数决定是否始终发送自检信息
            ///当参数为0时表示始终发送，其他值表示发送的最长时间
            ///if (!b_SelfCheckSendStatus)
            if (!g_stSACfg.ulSelfCheckMaxTime)
            {
                SendPASelfCheckMsg();
            }
            else
            {
                if (gos_get_uptime() <= g_stSACfg.ulSelfCheckMaxTime)
                {
                    SendPASelfCheckMsg();
                }
            }
            
        }

        if (!gos_simple_ping(g_stSACfg.stPISAddr.aucIP, 10))
        {
            GosLog(LOG_ERROR, "Ping IPH TCP  server "IP_FMT" failed", IP_ARG(g_stSACfg.stPISAddr.aucIP));
            continue;
        }

        if (InitIPHServerSocket())
        {
            SendPISHeartBeatMsg();
        }

        // 失败/超时，继续
        ulRecvRet = RecvSocketMsg(ulTimeout);
        if (ulRecvRet == RECV_FAIL)
        {
            gos_sleep_1ms(1);
            continue;
        }
        else if (ulRecvRet == RECV_TIMEOUT)
        {
            continue;
        }
        else if (ulRecvRet == RECV_MSG_INVALID)
        {
            gos_sleep_1ms(1);
            continue;
        }

        if (ulRecvRet == RECV_DIS_SUCC)
        {
            OnDISMsg((DTP_MSG_HDR_T*)g_aucDISMsg);
        }
        else if (ulRecvRet == RECV_PIS_SUCC)
        {
            OnPISMsg((PIS_MSG_HDR_T*)g_aucPISMsg);
            g_ulIPHLinkcheckMaxNum = 0;
        }
        else if (ulRecvRet == RECV_PA_SUCC)
        {
            OnPAMsg((PA_MSG_HDR_T*)g_aucPAMsg);
            g_ulPALinkcheckMaxNum = 0;
        }

        if ((gos_get_uptime_1ms() - ulLastCheckPAHeartBeatTime) > (g_stSACfg.ulPISHeartBeatTime*1000))
        {
            ulLastCheckPAHeartBeatTime = gos_get_uptime_1ms();

            g_ulPALinkcheckMaxNum++;
            if (g_ulPALinkcheckMaxNum >= 5)
            {
                GosLog(LOG_ERROR, "PA heartbeat Timeout");
                InitPASocket();
            }
        }

        if ((gos_get_uptime_1ms() - ulLastCheckIPHHeartBeatTime) > (g_stSACfg.ulPISHeartBeatTime*1000))
        {
            ulLastCheckIPHHeartBeatTime = gos_get_uptime_1ms();

            g_ulIPHLinkcheckMaxNum++;
            if (g_ulIPHLinkcheckMaxNum >= 5)
            {
                GosLog(LOG_ERROR, "IPH heartbeat Timeout");
                InitPISSocket();
            }
        }
    }
}

int main(int argc, char *argv[])
{
    if (gos_is_proc_running())
    {
        printf("sa is already running!\n");
        return -1;
    }

    gos_init_socket();

    SysInit();

    GLogFactory::GetInstance()->GetDefaultLogger()->SetLogLevel(LOG_DETAIL);
  //   test_buf();

    GosLog(LOG_INFO, "sa version : %s", g_acSAVersion);

    GosLog(LOG_INFO, "sa start time is %u", gos_get_sys_uptime_1ms());

    ServiceAgentMain();
}
