#include "g_include.h"
#include "ds_public.h"
#include "msg.h"
#include "ThreadCheck.h"

#ifdef _OSWIN32_
typedef struct ip_mreq
{
    struct in_addr  imr_multiaddr; //多播组的IP地址
    struct in_addr  imr_interface; //加入的客服端主机IP地址
}IP_MREQ;

#ifndef IP_ADD_MEMBERSHIP
// from ws2ipdef.h
#define IP_MULTICAST_TTL          10 // IP multicast TTL (hop limit).
#define IP_MULTICAST_LOOP         11 // IP multicast loopback.
#define IP_ADD_MEMBERSHIP         12 // Add an IP group membership.
#define IP_DROP_MEMBERSHIP        13 // Drop an IP group membership.

#endif

#else
typedef struct ip_mreq  IP_MREQ;
#endif

ThreadCheck     *g_ThreadCheck  = NULL;

UINT8           g_aucMulticastAddr[] = {224,9,0,0};     // 组播地址

UINT8 GetNetMaskBitsNum(UINT8 *pucNetmask, UINT32 &ulNetmaskBitsNum)
{
    UINT32  ulNetmask = ntohl(GET_INT(pucNetmask));
    UINT32  ulBit;

    ulNetmaskBitsNum = 32;
    while(ulNetmask > 0)
    {
        ulBit = (ulNetmask & 1);
        if (ulBit == 0)
        {
            ulNetmaskBitsNum--;
            ulNetmask = (ulNetmask>>1);
            continue;
        }
        
        break;
    }

    while(ulNetmask > 0)
    {
        ulBit = (ulNetmask & 1);
        if (ulBit == 0)
        {
            return FALSE;
        }

        ulNetmask = (ulNetmask>>1);
    }

    return TRUE;
}

BOOL IsLocalAddr(UINT8 *pucAddr)
{
    UINT8   aucLoopAddr[] = {127,0,0,1};

    if (memcmp(pucAddr, aucLoopAddr, sizeof(aucLoopAddr)) == 0)
    {
        return TRUE;
    }

#ifdef _OSWIN32_
    if (!gos_is_local_ip(pucAddr))
    {
        return FALSE;
    }

    CHAR    acCmd[256];
    CHAR    acFile[256];
    FILE    *fp = NULL;
    CHAR    *szInfo = NULL;
    INT32   iSize;
    CHAR    acKey[64];
    BOOL    bRet = FALSE;
    CHAR    acProcName[256] = {0};

    gos_get_proc_name(acProcName, FALSE);

    sprintf(acFile, "%s/ip_%s.info", gos_get_root_path(), acProcName);
    gos_delete_file(acFile);

    sprintf(acCmd, "ipconfig >%s", acFile);
    gos_system(acCmd);

    fp = fopen(acFile, "rb");
    if (!fp)
    {
        GosLog(LOG_ERROR, "open file %s failed", acFile);
        return FALSE;
    }

    fseek(fp, 0, SEEK_END);
    iSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (iSize <= 10)
    {
        goto end;
    }

    szInfo = (CHAR*)calloc(1, iSize + 2);
    if (!szInfo)
    {
        goto end;
    }

    fread(szInfo, 1, iSize, fp);
    sprintf(acKey, IP_FMT, IP_ARG(pucAddr));
    if (strstr(szInfo, acKey))
    {
        bRet = TRUE;
    }

    free(szInfo);

end:
    if (fp)
    {
        fclose(fp);

        gos_delete_file(acFile);
    }

    return bRet;
#else
    return gos_is_local_ip(pucAddr);
#endif
}

BOOL LoadClusterConfig(CHAR *szCfgFile, CLUSTER_CFG_T &stClusterCfg)
{
    CHAR        acFile[512];
    GConfFile   *pConf;
    BOOL        bRet = FALSE;
    CHAR        *szValue = NULL;

    sprintf(acFile, "%s/%s", gos_get_root_path(), szCfgFile);

    if (!gos_file_exist(acFile))
    {
        GosLog(LOG_ERROR, "file %s does not exist", acFile);
        return FALSE;
    }

    pConf = new GConfFile(acFile);

    stClusterCfg.bMulticastMode = FALSE;

    if (!pConf->Get("Cluster", "default_status", CLUSTER_STATUS_STAND_ALONE, &stClusterCfg.ulDefaultStatus))
    {
        GosLog(LOG_ERROR, "invalid default_status in [Cluster] of %s", szCfgFile);
        goto end;
    }

    if (stClusterCfg.ulDefaultStatus == CLUSTER_STATUS_STAND_ALONE)
    {
        GosLog(LOG_INFO, "running under stand alone mode");

        bRet = TRUE;
        goto end;
    }

    // proc_name
    if (!pConf->Get("Cluster", "proc_name", stClusterCfg.acProcName, sizeof(stClusterCfg.acProcName)-strlen(".exe")))
    {
        GosLog(LOG_ERROR, "invalid proc_name in [Cluster] of %s", szCfgFile);
        goto end;
    }

#ifdef _OSWIN32_
    if (!gos_str_end_like(stClusterCfg.acProcName, ".exe"))
    {
        strcat(stClusterCfg.acProcName, ".exe");
    }
#endif

    GosLog(LOG_INFO, "proc_name in [Cluster] is %s", stClusterCfg.acProcName);

    // udp_port
    if (!pConf->Get("Cluster", "udp_port", CLUSTER_MULTICAST_PORT, &stClusterCfg.usPort))
    {
        GosLog(LOG_ERROR, "invalid udp_port in [Cluster] of %s", szCfgFile);
        goto end;
    }

    GosLog(LOG_INFO, "udp_port in [Cluster] is %u", stClusterCfg.usPort);

    // peer_addr
    szValue = pConf->Get("Cluster", "peer_addr");
    if (szValue)
    {
        if (!gos_get_ip(szValue, stClusterCfg.aucPeerAddr))
        {
            GosLog(LOG_ERROR, "invalid peer_addr in [Cluster] of %s", szCfgFile);
            goto end;
        }

        GosLog(LOG_INFO, "peer_addr in [Cluster] is "IP_FMT, IP_ARG(stClusterCfg.aucPeerAddr));
    }
    else
    {
        if (stClusterCfg.bMulticastMode)
        {
            memcpy(stClusterCfg.aucPeerAddr, CLUSTER_MULTICAST_ADDR, 4);
        }
        else
        {
            GosLog(LOG_ERROR, "no peer_addr in [Cluster] of %s", szCfgFile);
            goto end;
        }
    }

    // if_name
    szValue = pConf->Get("Cluster", "if_name");
    if (!szValue)
    {
        GosLog(LOG_ERROR, "no if_name in [Cluster] of %s", szCfgFile);
        goto end;
    }

    stClusterCfg.szIfName = strdup(szValue);

    GosLog(LOG_INFO, "if_name in [Cluster] is %s", stClusterCfg.szIfName);

    // master_addr
    szValue = pConf->Get("Cluster", "master_addr");
    if (!szValue)
    {
        GosLog(LOG_ERROR, "no master_addr in [Cluster] of %s", szCfgFile);
        goto end;
    }

    if (!gos_get_ip(szValue, stClusterCfg.aucMasterAddr))
    {
        GosLog(LOG_ERROR, "invalid master_addr in [Cluster] of %s", szCfgFile);
        goto end;
    }

    GosLog(LOG_INFO, "master_addr in [Cluster] is "IP_FMT, IP_ARG(stClusterCfg.aucMasterAddr));

    // slave_addr
    szValue = pConf->Get("Cluster", "slave_addr");
    if (!szValue)
    {
        GosLog(LOG_ERROR, "no slave_addr in [Cluster] of %s", szCfgFile);
        goto end;
    }

    if (!gos_get_ip(szValue, stClusterCfg.aucSlaveAddr))
    {
        GosLog(LOG_ERROR, "invalid slave_addr in [Cluster] of %s", szCfgFile);
        goto end;
    }

    GosLog(LOG_INFO, "slave_addr in [Cluster] is "IP_FMT, IP_ARG(stClusterCfg.aucSlaveAddr));

    if (GET_INT(stClusterCfg.aucMasterAddr) == GET_INT(stClusterCfg.aucSlaveAddr))
    {
        GosLog(LOG_ERROR, "master_addr is same with slave_addr");
        goto end;
    }

    // gateway
    szValue = pConf->Get("Cluster", "gateway");
    if (!szValue)
    {
        GosLog(LOG_ERROR, "no gateway in [Cluster] of %s", szCfgFile);
        goto end;
    }

    if (!gos_get_ip(szValue, stClusterCfg.aucGateway))
    {
        GosLog(LOG_ERROR, "invalid gateway in [Cluster] of %s", szCfgFile);
        goto end;
    }

    GosLog(LOG_INFO, "gateway in [Cluster] is "IP_FMT, IP_ARG(stClusterCfg.aucGateway));

    // netmask
    szValue = pConf->Get("Cluster", "netmask");
    if (!szValue)
    {
        szValue = (CHAR*)"255.255.255.0";
    }

    if (!gos_get_ip(szValue, stClusterCfg.aucNetmask))
    {
        GosLog(LOG_ERROR, "invalid netmask in [Cluster] of %s", szCfgFile);
        goto end;
    }

    if (!GetNetMaskBitsNum(stClusterCfg.aucNetmask, stClusterCfg.ulNetmask))
    {
        GosLog(LOG_ERROR, "invalid netmask in [Cluster] of %s", szCfgFile);
        goto end;
    }

    GosLog(LOG_INFO, "netmask in [Cluster] is "IP_FMT, IP_ARG(stClusterCfg.aucNetmask));

    // check_time
    stClusterCfg.ulSwitchCheckTime = 15;
    if (!pConf->Get("Cluster", "check_time", 15, &stClusterCfg.ulSwitchCheckTime))
    {
        GosLog(LOG_ERROR, "invalid check_time in [Cluster] of %s", szCfgFile);
        goto end;
    }

    if (stClusterCfg.ulSwitchCheckTime < 10)
    {
        stClusterCfg.ulSwitchCheckTime = 10;
    }

    GosLog(LOG_INFO, "check_time in [Cluster] is %u", stClusterCfg.ulSwitchCheckTime);

    // status_file
    szValue = pConf->Get("status_file");
    if (szValue)
    {
        if (strlen(szValue) >= sizeof(stClusterCfg.acStatusFile))
        {
            GosLog(LOG_INFO, "status_file in [Cluster] is too long");
            goto end;
        }

#ifdef _OSWIN32_
        if (strlen(szValue) >= 3 &&
            (szValue[2] == '\\' ||
             szValue[2] == '/' ) )
#else
        if (strlen(szValue) >= 1 &&
            szValue[0] == '/' )
#endif
        {
            strcpy(stClusterCfg.acStatusFile, szValue);
        }
        else
        {
            sprintf(stClusterCfg.acStatusFile, "%s/%s", gos_get_root_path(), szValue);
        }        

        GosLog(LOG_INFO, "status_file in [Cluster] is %s", stClusterCfg.acStatusFile);
    }

    // add_ip_wait_time
    stClusterCfg.ulAddIPWaitTime = 15;
    if (!pConf->Get("Cluster", "add_ip_wait_time", 15, &stClusterCfg.ulAddIPWaitTime))
    {
        GosLog(LOG_ERROR, "invalid add_ip_wait_time in [Cluster] of %s", szCfgFile);
        goto end;
    }

    if (stClusterCfg.ulAddIPWaitTime < 10)
    {
        stClusterCfg.ulAddIPWaitTime = 10;
    }

    GosLog(LOG_INFO, "add_ip_wait_time in [Cluster] is %u", stClusterCfg.ulAddIPWaitTime);

    // addr_delay_set_time
    stClusterCfg.ulAddrDelaySetTime = 2;
    if (!pConf->Get("Cluster", "addr_delay_set_time", 2, &stClusterCfg.ulAddrDelaySetTime))
    {
        GosLog(LOG_ERROR, "invalid addr_delay_set_time in [Cluster] of %s", szCfgFile);
        goto end;
    }

    if (stClusterCfg.ulAddrDelaySetTime > 10)
    {
        stClusterCfg.ulAddrDelaySetTime = 10;
    }

    GosLog(LOG_INFO, "addr_delay_set_time in [Cluster] is %u", stClusterCfg.ulAddrDelaySetTime);

    bRet = TRUE;

end:
    delete pConf;
    return bRet;
}

BOOL StartThreadCheck(CLUSTER_CFG_T &stClusterCfg)
{
    if (stClusterCfg.ulDefaultStatus == CLUSTER_STATUS_STAND_ALONE)
    {
        return TRUE;
    }

    g_ThreadCheck = new ThreadCheck(stClusterCfg);
    g_ThreadCheck->Start();
    GosLog(LOG_FATAL, "thread check started");

    return TRUE;
}

VOID ThreadCheck::Log(UINT32 ulLogLevel, const CHAR *szLog, ...)
{
    CHAR            acLog[4096];
    va_list         vaLog;

    va_start(vaLog, szLog);

    memset(acLog, 0, sizeof(acLog));
    vsnprintf(acLog, sizeof(acLog)-1, szLog, vaLog);

    va_end(vaLog);

    GosLog(m_acLogName, ulLogLevel, acLog);
}

ThreadCheck::ThreadCheck(CLUSTER_CFG_T &stClusterCfg):GThread(NULL)
{
    memcpy(&m_stClusterCfg, &stClusterCfg, sizeof(m_stClusterCfg));

#ifdef _OSWIN32_
    if (m_stClusterCfg.ulDefaultStatus == CLUSTER_STATUS_MASTER)
    {
        strcpy(m_acLogName, "MASTER");
    }
    else
    {
        strcpy(m_acLogName, "SLAVE");
    }
#else
    strcpy(m_acLogName, "CHECK");
#endif

    m_bEnableRecvMsg = TRUE;
    m_bEnableSendMsg = TRUE;

    m_ulDefaultStatus = m_stClusterCfg.ulDefaultStatus;
    m_ulRunStatus = CLUSTER_STATUS_SLAVE;
    m_ulPeerRunStatus = CLUSTER_STATUS_MASTER;
    m_bIsPeerRunning = TRUE;

    m_ulLastSendCheckTime = 0;
    m_ulLastRecvCheckTime = 0;
    m_ulFirstCheckStatusFileFailTime = 0;

    m_bRunning = TRUE;

    m_stSock = INVALID_SOCKET;

    memset(&m_stPeerAddr, 0, sizeof(m_stPeerAddr));
    m_stPeerAddr.sin_family = AF_INET;
    m_stPeerAddr.sin_port = htons(m_stClusterCfg.usPort);
    memcpy(&m_stPeerAddr.sin_addr.s_addr, m_stClusterCfg.aucPeerAddr, 4);
    if (m_stClusterCfg.bMulticastMode)
    {
        memcpy(&m_stPeerAddr.sin_addr.s_addr, g_aucMulticastAddr, 4);
    }

    if (m_ulDefaultStatus == CLUSTER_STATUS_SLAVE)
    {
        if (!IsLocalAddr(m_stClusterCfg.aucSlaveAddr))
        {
            GosLog(LOG_INFO, "auto add slave ip");
            AddIP(m_stClusterCfg.aucSlaveAddr);

            if (IsLocalAddr(m_stClusterCfg.aucSlaveAddr))
            {
                GosLog(LOG_INFO, "auto add slave ip "IP_FMT " succ", IP_ARG(m_stClusterCfg.aucSlaveAddr));
            }
            else
            {
                GosLog(LOG_ERROR, "auto add slave ip "IP_FMT " fail", IP_ARG(m_stClusterCfg.aucSlaveAddr));
            }
        }
    }
    else
    {
#ifdef _OSWIN32_
        CHAR    acCmd[256];

        sprintf(acCmd, "netsh interface set interface \"%s\" enabled", m_stClusterCfg.szIfName);
        gos_system(acCmd);
#endif
    }
}

ThreadCheck::~ThreadCheck()
{
    CLOSE_SOCKET(m_stSock);
}

BOOL ThreadCheck::Init()
{
    if (m_stSock != INVALID_SOCKET)
    {
        return TRUE;
    }

    if (!m_stClusterCfg.bMulticastMode)
    {
        DOUBLE  dDelay;
        UINT16  usSeq;
        UINT32  ulTTL;
        UINT8   aucLocalAddr[4] = {0,0,0,0};

        if (!gos_ping(m_stClusterCfg.aucPeerAddr, 32, 1000, &dDelay, &usSeq, &ulTTL, aucLocalAddr))
        {
            Log(LOG_ERROR, "peer addr is not reachable");
         //   return FALSE;
        }

        m_stSock = gos_create_udp_server_socket(aucLocalAddr, m_stClusterCfg.usPort);
        if (m_stSock == INVALID_SOCKET)
        {
            Log(LOG_ERROR, "create checking udp socket on port %u failed", m_stClusterCfg.usPort);
            return FALSE;
        }

        Log(LOG_INFO, "thread check listen on "IP_FMT":%u", IP_ARG(aucLocalAddr), m_stClusterCfg.usPort);
        return TRUE;
    }

    SOCKET      stSock;
    SOCKADDR_IN stLocalAddr;
    IP_MREQ     stMulticast;
    UINT8       aucLocalAddr[] = {0,0,0,0};
    INT32       iReuse = 1;
    UINT8       ucLoop;

    stSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(stSock <= 0)
    {
        Log(LOG_ERROR, "create multicast socket failed");
        return FALSE;
    }

    /*端口释放后立即就可以被再次使用*/
    if (setsockopt(stSock, SOL_SOCKET, SO_REUSEADDR, (char *)&iReuse, sizeof(iReuse)) < 0)
    {
        Log(LOG_ERROR, "Set SO_REUSEADDR failed on multicast socket");
        goto err;
    }

    memset(&stLocalAddr, 0, sizeof(stLocalAddr));
    stLocalAddr.sin_family = AF_INET;
    stLocalAddr.sin_port = htons(m_stClusterCfg.usPort);
    memcpy(&stLocalAddr.sin_addr.s_addr, aucLocalAddr, 4);

    memset(&stMulticast, 0, sizeof(stMulticast));
    stMulticast.imr_multiaddr.s_addr = inet_addr(PIS_MULTICAST_ADDR);
    memcpy(&stMulticast.imr_interface.s_addr, aucLocalAddr, 4);

    if (setsockopt(stSock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&stMulticast, sizeof(stMulticast)) < 0)
    {
        Log(LOG_ERROR, "Set IP_ADD_MEMBERSHIP failed on multicast socket");
        goto err;
    }

    ucLoop = 0;
    if (setsockopt(stSock, IPPROTO_IP, IP_MULTICAST_LOOP, (char*)&ucLoop, sizeof(ucLoop)) < 0)
    {
        Log(LOG_ERROR, "Set IP_MULTICAST_LOOP failed on multicast socket");
        goto err;
    }

#ifdef _OSWIN32_
    if (bind(stSock, (struct sockaddr*)&stLocalAddr, sizeof(stLocalAddr)) != 0)
    {
        Log(LOG_ERROR, "bind local addr(" IP_FMT ") to multicast socket failed: %s", IP_ARG(aucLocalAddr), gos_get_socket_err());
        goto err;
    }
#else
    if (bind(stSock, (struct sockaddr*)&m_stPeerAddr, sizeof(m_stPeerAddr)) != 0)
    {
        Log(LOG_ERROR, "bind addr(" IP_FMT ") to multicast socket failed: %s", IP_ARG(&m_stPeerAddr.sin_addr.s_addr), gos_get_socket_err());
        goto err;
    }
#endif

    Log(LOG_INFO, "create socket for cluster status checking succ");
    m_stSock = stSock;

    return TRUE;

err:
    closesocket(stSock);

    return FALSE;
}

VOID ThreadCheck::Stop()
{
    m_bRunning = FALSE;
}

INT32 ThreadCheck::RecvMsg()
{
    if (!m_stClusterCfg.bMulticastMode)
    {
        UINT32      ulTimeout = 1000;
        SOCKADDR_IN stClientAddr;

        return gos_recv_udp_msg(m_stSock, m_aucMsg, sizeof(m_aucMsg), ulTimeout, &stClientAddr);
    }

    TIMEVAL     tv;
    FD_SET      fds;
    INT32       iRet;
    INT32       iRecvLen;

    FD_ZERO(&fds);
    FD_SET(m_stSock, &fds);
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    iRet = select(m_stSock + 1, &fds, NULL, NULL, &tv);

    if (iRet < 0)
    {
        if (errno == EINTR)
        {
            return 0;
        }

        Log(LOG_ERROR, "select error on multicast socket: %s", gos_get_socket_err());
        return -1;
    }
    else if (iRet == 0)
    {
        return 0;
    }

    if (!FD_ISSET(m_stSock, &fds))
    {
        return -1;
    }

    memset(m_aucMsg, 0, sizeof(m_aucMsg));
    iRecvLen = recv(m_stSock, (char*)m_aucMsg, sizeof(m_aucMsg), 0);
    if (iRecvLen < 0)
    {
        Log(LOG_ERROR, "recv multicast msg failed: %s", gos_get_socket_err());
    }
    else
    {
        Log(LOG_DETAIL, "recv multicast msg: %d", iRecvLen);
    }

    return iRecvLen;
}

GOS_THREAD_RET ThreadCheck::ThreadEntry(void* pPara)
{
    INT32       iRet;
    UINT32      ulTime;
    UINT32      ulStartTime = gos_get_uptime_1ms();
    CHECK_MSG_HDR   *pstMsgHdr = (CHECK_MSG_HDR*)m_aucMsg;

    Log(LOG_INFO, "thread check start running");

    while(m_bRunning)
    {
        if (!Init())
        {
            gos_sleep(3);
            continue;
        }

        ulTime = gos_get_uptime_1ms() - ulStartTime;
        if (ulTime >= 1000)
        {
            SendCheckMsg();
            ulStartTime = gos_get_uptime_1ms();

            Check();
        }

        if (!m_bEnableRecvMsg)
        {
            gos_sleep_1ms(10);
            continue;
        }

        iRet = RecvMsg();
        if (iRet <= 0)
        {
            continue;
        }

        if (iRet < (INT32)sizeof(CHECK_MSG_HDR))
        {
            Log(LOG_ERROR, "invalid check msg len: %d", iRet);
            continue;
        }

        NTOHS_IT(pstMsgHdr->usMsgType);
        NTOHS_IT(pstMsgHdr->usMsgLen);

        if ((UINT32)iRet != (sizeof(CHECK_MSG_HDR) + pstMsgHdr->usMsgLen))
        {
            Log(LOG_ERROR, "invalid check msg len: %d", iRet);
            continue;
        }

        if (memcmp(pstMsgHdr->acMsgMagic, CHECK_MSG_MAGIC, sizeof(pstMsgHdr->acMsgMagic)) != 0)
        {
            Log(LOG_ERROR, "invalid check msg magic");
            continue;
        }

        OnRecvMsg(pstMsgHdr->usMsgType, pstMsgHdr+1, pstMsgHdr->usMsgLen);
    }

    CLOSE_SOCKET(m_stSock);

    return 0;
}

VOID ThreadCheck::OnRecvMsg(UINT16 usMsgType, VOID *pvMsg, UINT16 usMsgLen)
{
    if (usMsgType == EV_CHECK_STATUS_REPORT_IND)
    {
        OnCheckStatusReportInd(pvMsg, usMsgLen);
    }
    else
    {
        Log(LOG_ERROR, "invalid check msg tpye %u", usMsgType);
    }
}

VOID ThreadCheck::OnCheckStatusReportInd(VOID *pvMsg, UINT16 usMsgLen)
{
    CHECK_STATUS_REPORT_IND_T   *pstMsg = (CHECK_STATUS_REPORT_IND_T*)pvMsg;

    if (usMsgLen != sizeof(CHECK_STATUS_REPORT_IND_T))
    {
        Log(LOG_ERROR, "invalid msg len(%u) of CHECK_STATUS_REPORT_IND", usMsgLen);
        return;
    }

    if (m_ulPeerRunStatus != pstMsg->ucRunStatus ||
        m_bIsPeerRunning != pstMsg->ucRunning )
    {
        Log(LOG_FATAL, "peer status is %u, it's %s", pstMsg->ucRunStatus,
                pstMsg->ucRunning? "running":"not running");
    }

    m_ulPeerRunStatus = pstMsg->ucRunStatus;
    m_bIsPeerRunning  = pstMsg->ucRunning;

    m_ulLastRecvCheckTime = gos_get_uptime();

    Log(LOG_DETAIL, "recv check msg at %u", m_ulLastRecvCheckTime);
}

BOOL ThreadCheck::SendMsg(UINT16 usMsgType, VOID *pvMsg, UINT16 usMsgLen)
{
    if (!m_bEnableSendMsg)
    {
        return FALSE;
    }

    UINT32          ulMsgLen = sizeof(CHECK_MSG_HDR) + usMsgLen;
    UINT8           *pucMsg = (UINT8*)calloc(1, ulMsgLen);
    CHECK_MSG_HDR   *pstMsgHdr = (CHECK_MSG_HDR*)pucMsg;

    memcpy(pstMsgHdr->acMsgMagic, CHECK_MSG_MAGIC, sizeof(pstMsgHdr->acMsgMagic));
    pstMsgHdr->usMsgType = HTONS(usMsgType);
    pstMsgHdr->usMsgLen = HTONS(usMsgLen);
    memcpy(pstMsgHdr+1, pvMsg, usMsgLen);

    if (!gos_udp_send(m_stSock, (void *)pucMsg, ulMsgLen, &m_stPeerAddr))
    {
        Log(LOG_ERROR, "send msg to peer failed: %s", gos_get_socket_err());
        return FALSE;
    }

    return TRUE;
}

BOOL ThreadCheck::IsLocalRunning()
{
    if (!gos_is_proc_exists(m_stClusterCfg.acProcName, 0))
    {
        GosLog(LOG_ERROR, "%s is not running", m_stClusterCfg.acProcName);
        return FALSE;
    }

    // 超过规定的时间持续检测失败才认为被监控进程异常
    if (m_stClusterCfg.acStatusFile[0])
    {
        INT32   iSize = gos_get_file_size(m_stClusterCfg.acStatusFile);

        if (iSize < 0 || // 文件不存在
            iSize > 0)   // 文件有内容（表示失败）
        {
            UINT32  ulTime = gos_get_uptime();

            if (m_ulFirstCheckStatusFileFailTime == 0)
            {
                m_ulFirstCheckStatusFileFailTime = ulTime;
            }

            if (iSize > 0)   // 文件有内容（表示失败）
            {
                if ((UINT32)(ulTime - m_ulFirstCheckStatusFileFailTime) >= 3)
                {
                    GosLog(LOG_ERROR, "%s is running with error", m_stClusterCfg.acProcName);
                }

                // 避免被监控进程假死、死循环时不再写文件
                gos_delete_file(m_stClusterCfg.acProcName);
            }

            if ((UINT32)(ulTime - m_ulFirstCheckStatusFileFailTime) >= m_stClusterCfg.ulSwitchCheckTime)
            {
                return FALSE;
            }
        }
        else
        {
            m_ulFirstCheckStatusFileFailTime = 0;
        }
    }
    
    return TRUE;
}

VOID ThreadCheck::SendCheckMsg()
{
    CHECK_STATUS_REPORT_IND_T   stMsg = {0};

    if (IsLocalAddr(m_stClusterCfg.aucMasterAddr))
    {
        stMsg.ucRunStatus = CLUSTER_STATUS_MASTER;
    }
    else
    {
        stMsg.ucRunStatus = CLUSTER_STATUS_SLAVE;
    }

    stMsg.ucRunning = IsLocalRunning();

    SendMsg(EV_CHECK_STATUS_REPORT_IND, &stMsg, sizeof(stMsg));
    m_ulLastSendCheckTime = gos_get_uptime();
    if (0 == m_ulLastRecvCheckTime)
    {
        m_ulLastRecvCheckTime = m_ulLastSendCheckTime;
    }

    Log(LOG_DETAIL, "send check msg at %u, curr status is %u", m_ulLastSendCheckTime, m_ulRunStatus);
}

const CHAR* ThreadCheck::GetPeerRunning()
{
    if (m_ulPeerRunStatus != CLUSTER_STATUS_CHECK_FAIL &&
        m_bIsPeerRunning)
    {
        return "running";
    }
    else
    {
        return "not running";
    }
}

VOID ThreadCheck::Check()
{
    BOOL    bIsLocalRunning = IsLocalRunning();

    // 都没有运行，保留现状
    if (!m_bIsPeerRunning && !bIsLocalRunning)
    {
        Log(LOG_ERROR, "local is not running");
        return;
    }

    if (m_ulLastSendCheckTime > 0 &&
        m_ulLastSendCheckTime >= (m_stClusterCfg.ulSwitchCheckTime+m_ulLastRecvCheckTime))
    {
        if (m_ulPeerRunStatus != CLUSTER_STATUS_CHECK_FAIL)
        {
            Log(LOG_ERROR, "check peer failed");
        }

        m_ulPeerRunStatus = CLUSTER_STATUS_CHECK_FAIL;
        m_bIsPeerRunning = FALSE;
    }

    if (m_ulDefaultStatus == CLUSTER_STATUS_MASTER)
    {
        if (bIsLocalRunning)
        {
            SwitchToMaster();
        }
        else if (m_bIsPeerRunning)
        {
            SwitchToSlave();
        }
    }
    else
    {
        if (m_bIsPeerRunning)
        {
            SwitchToSlave();
        }
        else if (bIsLocalRunning)
        {
            SwitchToMaster();
        }
    }

    if (m_ulRunStatus == CLUSTER_STATUS_MASTER)
    {
        if (IsLocalAddr(m_stClusterCfg.aucSlaveAddr))
        {
            Log(LOG_FATAL, "auto disable slave ip under master mode");
            DeleteIP(m_stClusterCfg.aucSlaveAddr);

            if (!IsLocalAddr(m_stClusterCfg.aucSlaveAddr))
            {
                GosLog(LOG_INFO, "auto disable slave ip "IP_FMT " succ", IP_ARG(m_stClusterCfg.aucSlaveAddr));
            }
            else
            {
                GosLog(LOG_ERROR, "auto disable slave ip "IP_FMT " fail", IP_ARG(m_stClusterCfg.aucSlaveAddr));
            } 

        }

        if (bIsLocalRunning)
        {
            if (!IsLocalAddr(m_stClusterCfg.aucMasterAddr))
            {
                gos_sleep(m_stClusterCfg.ulAddrDelaySetTime);

                Log(LOG_FATAL, "auto add master ip under master mode");
                AddIP(m_stClusterCfg.aucMasterAddr);

                if (IsLocalAddr(m_stClusterCfg.aucMasterAddr))
                {
                    GosLog(LOG_INFO, "auto add master ip "IP_FMT " succ", IP_ARG(m_stClusterCfg.aucMasterAddr));
                }
                else
                {
                    GosLog(LOG_ERROR, "auto add master ip "IP_FMT " fail", IP_ARG(m_stClusterCfg.aucMasterAddr));
                }
            }
        }
    }
    else
    {
        if (IsLocalAddr(m_stClusterCfg.aucMasterAddr))
        {
            Log(LOG_FATAL, "auto disable master ip under slave mode");
            DeleteIP(m_stClusterCfg.aucMasterAddr);

            if (!IsLocalAddr(m_stClusterCfg.aucMasterAddr))
            {
                GosLog(LOG_INFO, "auto disable master ip "IP_FMT " succ", IP_ARG(m_stClusterCfg.aucMasterAddr));
            }
            else
            {
                GosLog(LOG_ERROR, "auto disable master ip "IP_FMT " fail", IP_ARG(m_stClusterCfg.aucMasterAddr));
            } 
        }

        if (!IsLocalAddr(m_stClusterCfg.aucSlaveAddr))
        {
            gos_sleep(m_stClusterCfg.ulAddrDelaySetTime);
            Log(LOG_FATAL, "auto add slave ip under slave mode");
            AddIP(m_stClusterCfg.aucSlaveAddr);

            if (IsLocalAddr(m_stClusterCfg.aucSlaveAddr))
            {
                GosLog(LOG_INFO, "auto add slave ip "IP_FMT " succ", IP_ARG(m_stClusterCfg.aucSlaveAddr));
            }
            else
            {
                GosLog(LOG_ERROR, "auto add slave ip "IP_FMT " fail", IP_ARG(m_stClusterCfg.aucSlaveAddr));
            }
        }
    }
}

VOID ThreadCheck::SwitchToMaster()
{
    if (m_ulRunStatus != CLUSTER_STATUS_MASTER)
    {
        Log(LOG_FATAL, "switch to master, peer status is %u, it's %s", m_ulPeerRunStatus, GetPeerRunning());
    }

    m_ulRunStatus = CLUSTER_STATUS_MASTER;
}

VOID ThreadCheck::SwitchToSlave()
{
    if (m_ulRunStatus != CLUSTER_STATUS_SLAVE)
    {
        Log(LOG_FATAL, "switch to slave, peer status is %u, it's %s", m_ulPeerRunStatus, GetPeerRunning());
    }

    m_ulRunStatus = CLUSTER_STATUS_SLAVE;
}

VOID PrintIfList()
{
    GOS_ADAPTER_INFO_T  astIfInfo[32];
    UINT32              ulIfNum = 0;

    if (!gos_get_adapter_info(astIfInfo, sizeof(astIfInfo), &ulIfNum))
    {
        return;
    }

    for (UINT32 i=0; i<ulIfNum; i++)
    {
        printf("if %u: %s " IP_FMT "\n", astIfInfo[i].ulIndex, astIfInfo[i].acName, IP_ARG(astIfInfo[i].aucAddr));
    }
}

VOID SetLinuxIP(CHAR *szIfName, BOOL bDel, UINT8 *pucAddr, UINT8 *pucGateway, UINT8 *pucNetmask, UINT32 ulNetmask)
{
    CHAR    acFile[256];
    CHAR    acCmd[256];
    FILE    *fp;

    sprintf(acFile, "/etc/sysconfig/network-scripts/ifcfg-%s", szIfName);
    fp = fopen(acFile, "w+t");
    if (!fp)
    {
        GosLog(LOG_ERROR, "open file %s failed", acFile);
        return;
    }

    fprintf(fp, "TYPE=Ethernet\n"
        "PROXY_METHOD=none\n"
        "BROWSER_ONLY=no\n"
        "BOOTPROTO=static\n"
        "DEFROUTE=yes\n"
        "IPV4_FAILURE_FATAL=no\n"
        "IPV6INIT=yes\n"
        "IPV6_AUTOCONF=yes\n"
        "IPV6_DEFROUTE=yes\n"
        "IPV6_FAILURE_FATAL=no\n"
        "IPV6_ADDR_GEN_MODE=stable-privacy\n"
        "NAME=%s\n"
        "DEVICE=%s\n"
        "ONBOOT=yes\n"
        "%sIPADDR=" IP_FMT "\n"
        "NETMASK=" IP_FMT "\n"
        "GATEWAY=" IP_FMT "\n"
        "PREFIX=%u\n",
        szIfName, szIfName,
        bDel?"#":"",
        IP_ARG(pucAddr), 
        IP_ARG(pucNetmask),
        IP_ARG(pucGateway),
        ulNetmask);

    fclose(fp);

    sprintf(acCmd, "ifdown %s", szIfName);
    gos_system(acCmd);

    sprintf(acCmd, "ifup %s", szIfName);
    gos_system(acCmd);

    gos_sleep(1);
}

BOOL AddWindowsIP(CHAR *szIfName, UINT8 *pucAddr, UINT8 *pucGateway, UINT8 *pucNetmask)
{
    CHAR    acCmd[256];
    CHAR    acFile[256];
    FILE    *fp = NULL;
    CHAR    *szInfo = NULL;
    INT32   iSize;
    BOOL    bRet = FALSE;

    sprintf(acFile, "%s/ip.info", gos_get_root_path());
    gos_delete_file(acFile);

    sprintf(acCmd, "netsh interface ip add address \"%s\" "IP_FMT " "IP_FMT " >%s",
            szIfName, IP_ARG(pucAddr), IP_ARG(pucNetmask), acFile);
    gos_system(acCmd);

    GosLog(LOG_INFO, "%s", acCmd);

    fp = fopen(acFile, "rb");
    if (!fp)
    {
        GosLog(LOG_ERROR, "open file %s failed", acFile);
        return FALSE;
    }

    fseek(fp, 0, SEEK_END);
    iSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    szInfo = (CHAR*)calloc(1, iSize + 2);
    if (!szInfo)
    {
        goto end;
    }

    if (iSize > 0)
    {
        fread(szInfo, 1, iSize, fp);
    }

    if (strstr(szInfo, "对象已存在"))
    {
        GosLog(LOG_WARN, "ip already exists, restart the interface");

        sprintf(acCmd, "netsh interface set interface \"%s\" disabled", szIfName);
        gos_system(acCmd);

        sprintf(acCmd, "netsh interface set interface \"%s\" enabled", szIfName);
        gos_system(acCmd);  
    }

    free(szInfo);

    bRet = TRUE;

end:
    if (fp)
    {
        fclose(fp);
    }

    return bRet;
}

BOOL ThreadCheck::AddIP(UINT8 *pucAddr)
{
    BOOL    bPingSucc = FALSE;

    for (UINT32 i=0; i<10; i++)
    {
        if (gos_simple_ping(pucAddr, 100))
        {
            GosLog(LOG_WARN, "addr(" IP_FMT ") still existed on other machine", IP_ARG(pucAddr));
            bPingSucc = TRUE;
            gos_sleep_1ms(200);
            continue;
        }

        break;
    }

    if (bPingSucc)
    {
        gos_sleep(1);
    }

#ifdef _OSWIN32_
    AddWindowsIP(m_stClusterCfg.szIfName, pucAddr, m_stClusterCfg.aucGateway, m_stClusterCfg.aucNetmask);
#else
    SetLinuxIP(m_stClusterCfg.szIfName, FALSE, pucAddr, m_stClusterCfg.aucGateway, m_stClusterCfg.aucNetmask, m_stClusterCfg.ulNetmask);
#endif

    UINT32  ulSleepTime = 200;

    for (UINT32 i=0; i<m_stClusterCfg.ulAddIPWaitTime*1000/ulSleepTime; i++)
    {
        if (IsLocalAddr(pucAddr))
        {
            return TRUE;
        }

        gos_sleep_1ms(ulSleepTime);
    }

    return FALSE;
}

BOOL ThreadCheck::DeleteIP(UINT8 *pucAddr)
{
#ifdef _OSWIN32_
    CHAR    acCmd[1024];

    sprintf(acCmd, "netsh interface ip del address \"%s\" "IP_FMT,
            m_stClusterCfg.szIfName, IP_ARG(pucAddr));
    gos_system(acCmd);

    return TRUE;
    //return !IsLocalAddr(pucAddr);
#else
    CHAR    acCmd[1024];

    sprintf(acCmd, "ip addr del " IP_FMT " dev %s", IP_ARG(pucAddr), m_stClusterCfg.szIfName);
    gos_system(acCmd);

    return TRUE;
#endif
}

void ThreadCheck::Test()
{
    UINT8       aucAddr[] = {192,0,1,121};

    GosLog(LOG_INFO, "add ip");

  /*  if (!DeleteIP(aucAddr))
    {
        GosLog(LOG_ERROR, "del ip failed");
    }*/
    if (IsLocalAddr(aucAddr))
    {

    }

    if (!AddIP(aucAddr))
    {
        GosLog(LOG_ERROR, "add ip failed");
    }
    else
    {
        GosLog(LOG_INFO, "add ip succ");
    }
}