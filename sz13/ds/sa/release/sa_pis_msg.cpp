#include "sa_public.h"

BOOL        g_bNeedSendSelfCheckMsg = TRUE;
UINT32      g_ulLastPISMsgTime = 0;

UINT32      g_ulTotalCheckPISCount = 0;

ID_VALUE_T  g_astRecBrdCfg[1024];
UINT32      g_ulRecBrdCfgNum = 0;

extern UINT8    g_aucLocalAddr[4];
extern UINT8    g_aucGateway[4];

extern SOCKET   g_stPATCPClientSock;

extern BOOL SendSAStartRecBrdRspMsg(PA_REC_BRD_RSP_T *pstRsp);
extern BOOL SendSAStartRealBrdRspMsg(PA_REAL_BRD_RSP_T *pstRsp);
extern BOOL SendSAStopRealBrdRspMsg(PA_STOP_REAL_BRD_RSP_T *pstRsp);
extern BOOL SendSAPAHeartBeatRspMsg(PA_GET_SYS_STATUS_RSP_T *pstRsp);

extern VOID InitPASocket();
extern VOID CheckResetButton();

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


BOOL DiscoverPISServer(UINT8 *pucLocalAddr, UINT8 *pucGateway, UINT8 *pucPISAddr)
{
    SOCKET      stSock;
    SOCKADDR_IN stLocalAddr;
    SOCKADDR_IN stPeerAddr;
    IP_MREQ     stMulticast;
    UINT8       aucRecvBuf[1024];
    TIMEVAL     tv;
    FD_SET      fds;
    INT32       iRet;
    INT32       iReuse = 1;
    INT32       iRecvLen;
    UINT32      i;
    BOOL        bRet = FALSE;
    UINT8       ucLoop;
    UINT32      usMsgLen;
    UINT32      usMsgType = EV_PIS_FIND_PIS;
    PIS_FIND_PIS_REQ_T  stReq = {0};
    PIS_FIND_PIS_RSP_T  *pstRsp = (PIS_FIND_PIS_RSP_T*)aucRecvBuf;

    //printf("dev discover start\n");

    usMsgLen = sizeof(PIS_FIND_PIS_REQ_T) - sizeof(PIS_MSG_HDR_T);
    stReq.stHdr.usMsgMagic = htons(PIS_MSG_MAGIC);
    stReq.stHdr.usMsgType = htons(usMsgType);
    stReq.stHdr.usMsgLen = htons(usMsgLen);

    memcpy(stReq.aucLocalAddr, pucLocalAddr, sizeof(stReq.aucLocalAddr));
    memcpy(stReq.aucPISAddr, pucPISAddr, sizeof(stReq.aucPISAddr));
    memcpy(stReq.aucGateway, pucGateway, sizeof(stReq.aucGateway));

    stSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(stSock <= 0)
    {
        GosLog(LOG_ERROR, "create multicast socket failed");
        return FALSE;
    }

    /*端口释放后立即就可以被再次使用*/
    if (setsockopt(stSock, SOL_SOCKET, SO_REUSEADDR, (char *)&iReuse, sizeof(iReuse)) < 0)
    {
        GosLog(LOG_ERROR, "Set SO_REUSEADDR failed on multicast socket");
        goto err;
    }

    memset(&stLocalAddr, 0, sizeof(stLocalAddr));
    stLocalAddr.sin_family = AF_INET;
    stLocalAddr.sin_port = htons(PIS_MULTICAST_PORT);
//  stAddr.sin_addr.s_addr = inet_addr(PIS_MULTICAST_ADDR);
    memcpy(&stLocalAddr.sin_addr.s_addr, pucLocalAddr, 4);

    memset(&stPeerAddr, 0, sizeof(stPeerAddr));
    stPeerAddr.sin_family = AF_INET;
    stPeerAddr.sin_port = htons(PIS_MULTICAST_PORT);
    stPeerAddr.sin_addr.s_addr = inet_addr(PIS_MULTICAST_ADDR);

    memset(&stMulticast, 0, sizeof(stMulticast));
    stMulticast.imr_multiaddr.s_addr = inet_addr(PIS_MULTICAST_ADDR);
    //stMulticast.imr_interface.s_addr = INADDR_ANY;        // 发送组播消息的源主机地址
    memcpy(&stMulticast.imr_interface.s_addr, pucLocalAddr, 4);

    if (setsockopt(stSock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&stMulticast, sizeof(stMulticast)) < 0)
        //if(setsockopt(stSock, IPPROTO_IP, 12, (char *)&stMulticast, sizeof(stMulticast)) < 0)
    {
        GosLog(LOG_ERROR, "Set IP_ADD_MEMBERSHIP failed on multicast socket");
        goto err;
    }

    ucLoop = 0;
    if (setsockopt(stSock, IPPROTO_IP, IP_MULTICAST_LOOP, (char*)&ucLoop, sizeof(ucLoop)) < 0)
    {
        GosLog(LOG_ERROR, "Set IP_MULTICAST_LOOP failed on multicast socket");
        goto err;
    }

#ifdef _OSWIN32_
    if (bind(stSock, (struct sockaddr*)&stLocalAddr, sizeof(stLocalAddr)) != 0)
    {
        GosLog(LOG_ERROR, "bind local addr(" IP_FMT ") to multicast socket failed: %s", IP_ARG(pucLocalAddr), gos_get_socket_err());
        goto err;
    }
#else
    if (bind(stSock, (struct sockaddr*)&stPeerAddr, sizeof(stPeerAddr)) != 0)
    {
        GosLog(LOG_ERROR, "bind addr(" IP_FMT ") to multicast socket failed: %s", IP_ARG(&stPeerAddr.sin_addr.s_addr), gos_get_socket_err());
        goto err;
    }
#endif

    gos_udp_send(stSock, &stReq, sizeof(stReq), &stPeerAddr);

    for (i=0; i<5; i++)
    {
        CheckResetButton();

        FD_ZERO(&fds);
        FD_SET(stSock, &fds);
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        iRet = select(stSock + 1, &fds, NULL, NULL, &tv);
        if (iRet < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }

            GosLog(LOG_ERROR, "select error on multicast socket: %s", gos_get_socket_err());
            goto err;
        }
        else if (iRet == 0)
        {
            GosLog(LOG_ERROR, "recv multicast timeout");
            continue;
        }

        if (FD_ISSET(stSock, &fds))
        {
            memset(aucRecvBuf, 0, sizeof(aucRecvBuf));
            iRecvLen = recv(stSock, (char*)aucRecvBuf, sizeof(PIS_FIND_PIS_RSP_T), 0);
            if (iRecvLen != sizeof(PIS_FIND_PIS_RSP_T))
            {
                GosLog(LOG_ERROR, "Recv msg len(%d) should be %u, sender addr is "IP_FMT,
                       iRecvLen, sizeof(PIS_FIND_PIS_RSP_T), IP_ARG(pstRsp->aucPISAddr));

                continue;
            }
            else
            {
                GosLog(LOG_INFO, "Recv PIS server ack msg, PIS addr is "IP_FMT, IP_ARG(pstRsp->aucPISAddr));

                if (memcmp(pucPISAddr, pstRsp->aucPISAddr, sizeof(pstRsp->aucPISAddr)) != 0)
                {
                    GosLog(LOG_ERROR, "PIS addr should be "IP_FMT, IP_ARG(pucPISAddr));
                    continue;
                    //goto err;
                }

                bRet = TRUE;
                break;
            }
        }
        else
        {
            GosLog(LOG_ERROR, "unknown socket recved: %u", stSock);
        }
    }

err:
    closesocket(stSock);
    return bRet;
}

BOOL SendPAMsg(UINT16 usMsgType, VOID *pvMsg, UINT16 usMsgLen)
{
    PA_MSG_HDR_T   *pstMsg = (PA_MSG_HDR_T*)calloc(1, sizeof(PA_MSG_HDR_T)+usMsgLen);

    if (!pstMsg)
    {
        GosLog(LOG_ERROR, "malloc failed when sending PA msg");
        return FALSE;
    }

    if (g_stPATCPClientSock == INVALID_SOCKET)
    {
        GosLog(LOG_ERROR, "sned PA msg failed for pa_tcp_socket is invalid");
        return FALSE;
    }

    UINT16    usMsgMagic = PIS_MSG_MAGIC;
    pstMsg->usMsgMagic = htons(usMsgMagic);//PIS_MSG_MAGIC;
    pstMsg->usMsgType = htons(usMsgType);
    pstMsg->usMsgLen = htons(usMsgLen);

    if (pvMsg && usMsgLen)
    {
        memcpy(pstMsg+1, pvMsg, usMsgLen);
    }

    if (!gos_tcp_send(g_stPATCPClientSock, pstMsg, sizeof(PA_MSG_HDR_T) + usMsgLen))
    {
        GosLog(LOG_ERROR, "send PA msg(%u) failed", usMsgType);
        InitPASocket();
        free(pstMsg);

        return FALSE;
    }

    free(pstMsg);

    return TRUE;
}

VOID SendPAHeartBeatMsg()
{
    UINT32      ulCurrTime = gos_get_uptime();

    if (g_ulLastPISMsgTime == 0 ||
        ulCurrTime >= (g_ulLastPISMsgTime+g_stSACfg.ulPISHeartBeatTime))
    {
        SendPAMsg(EV_PA_GET_SYS_STATUS);
    }
}

BOOL GetSelfCheckStatus(UINT8 *pucCheckStatus)
{
    UINT32  ulValue = 0;

    if (!CfgGet(APP_STATUS_SelfCheckStatus, &ulValue))
    {
        return FALSE;
    }

    if (ulValue > 0xff)
    {
        *pucCheckStatus = 0xff;
    }
    else
    {
        *pucCheckStatus = (UINT8)ulValue;
    }

    return TRUE;
}

VOID SendPASelfCheckMsg()
{
    static UINT32   ulSendTime = 0;
    UINT32          ulTime = gos_get_uptime() - ulSendTime;
    UINT32          ulSendPeriod = 5;

    if (ulTime >= ulSendPeriod)
    {
        PIS_SELFCHECK_STATUS_REQ_T  stMsg = {0};

        if (!GetSelfCheckStatus(&stMsg.ucCheckStatus))
        {
            GosLog(LOG_ERROR, "get self check status failed");
            return;
        }

        SendPAMsg(EV_PIS_SELFCHECK_STATUS, &stMsg, sizeof(stMsg));
        ulSendTime = gos_get_uptime();
    }

    GosLog(LOG_INFO, "send PA app self check status end");
}

VOID SendPAGetTrainInfoMsg()
{
    static UINT32   ulLastSendTime = 0;
    UINT32      ulCurrTime = gos_get_uptime();
    GJsonParam  Param;

    if (ulLastSendTime > 0 && ulCurrTime < (5+ulLastSendTime))
    {
        return;
    }

    GosLog(LOG_DETAIL, "send pis get train info msg");
    SendPAMsg(EV_PIS_GET_TRAIN_INFO);

    ulLastSendTime = gos_get_uptime();
    if (ulLastSendTime == 0)
    {
        ulLastSendTime = 1;
    }

    GosLog(LOG_DETAIL, "send pis get train info msg end");
}

VOID SendPARecBrdCfgMsg()
{
    SendPAMsg(EV_PA_REC_CFG);
}

/**
 * @brief           向PA服务器发送获取获取预录音广播配置请求
 * @return          VOID
 * @author          zhengwenjie(zhengwenjie@gbcom.com.cn)
 * @par 示例:
 * @code
 *
 * @endcode
 * @note
 * 根据文档选择
 */
VOID CheckPARecBrdCfg()
{
    static UINT32   ulLastSendTime = 0;
    UINT32      ulCurrTime = gos_get_uptime();

    if (g_ulTotalCheckPISCount >= 20)
    {
        return;
    }

    if (ulLastSendTime == 0 ||
        ulCurrTime >= (ulLastSendTime+15))
    {
        SendPARecBrdCfgMsg();
        GosLog(LOG_INFO, "send get pis rec brd cfg msg");

        ulLastSendTime = gos_get_uptime();
        if (ulLastSendTime == 0)
        {
            ulLastSendTime = 1;
        }

        g_ulTotalCheckPISCount++;
    }
}

VOID OnPAStartBrdRsp(VOID *pvMsg, UINT32 ulMsgLen)
{
    PA_REAL_BRD_RSP_T   *pstRsp = (PA_REAL_BRD_RSP_T*)pvMsg;

    GosLog(LOG_DETAIL, "recv PA_REAL_BRD_RSP msg");
    if (ulMsgLen != sizeof(PA_REAL_BRD_RSP_T))
    {
        GosLog(LOG_ERROR, "invalid msg len(%u) of PA_REAL_BRD_RSP", ulMsgLen);
        return;
    }

    // 发送消息给DIS
    SendSAStartRealBrdRspMsg(pstRsp);
}

VOID OnPAEndBrdRsp(VOID *pvMsg, UINT32 ulMsgLen)
{
    PA_STOP_REAL_BRD_RSP_T  *pstRsp = (PA_STOP_REAL_BRD_RSP_T*)pvMsg;

    GosLog(LOG_DETAIL, "recv PA_STOP_REAL_BRD_RSP_T msg");
    if (ulMsgLen != sizeof(PA_STOP_REAL_BRD_RSP_T))
    {
        GosLog(LOG_ERROR, "invalid msg len(%u) of PA_STOP_REAL_BRD_RSP_T", ulMsgLen);
        return;
    }

    // 发送消息给DIS
    SendSAStopRealBrdRspMsg(pstRsp);
}

/**
 * @brief           解析PA服务器发送的列车状态信息
 * @param           pvMsg       [in]
 * @param           ulMsgLen    [in]  在此条消息中该字段应为2
 * @return          VOID 
 * @author          zhengwenjie(zhengwenjie@gbcom.com.cn)
 * @par 示例:
 * @code
 *  
 * @endcode
 * @note
 * 设备盖状态：1：打开 0：关闭
 * 驾驶模式：1：有人驾驶 0：无人驾驶
 */
static VOID OnPAGetTrainInfoRsp(VOID *pvMsg, UINT32 ulMsgLen)
{
    static UINT8                ucCoverStatus = 0xff;
    static UINT8                ucDriverMode = 0xff;
    PIS_GET_TRAIN_INFO_RSP_T    *pstRsp = (PIS_GET_TRAIN_INFO_RSP_T*)pvMsg;

    GosLog(LOG_DETAIL, "recv PIS_GET_TRAIN_INFO msg");
    if (ulMsgLen != sizeof(PIS_GET_TRAIN_INFO_RSP_T))
    {
        GosLog(LOG_ERROR, "invalid msg len(%u) of GET_TRAIN_INFO", ulMsgLen);
        return;
    }

    // 存储列车状态
    if (pstRsp->ucCoverStatus != ucCoverStatus ||
        pstRsp->ucDriverMode  != ucDriverMode )
    {
        ucCoverStatus = pstRsp->ucCoverStatus;
        ucDriverMode  = pstRsp->ucDriverMode;

        GosLog(LOG_INFO, "now cover status is %u, driver mode is %u", ucCoverStatus, ucDriverMode);

        CfgSet(TRAIN_COVER_STATUS, ucCoverStatus);
        CfgSet(TRAIN_DRIVE_MODE, ucDriverMode);
//        CfgCommit();
    }

    return ;
}

static VOID OnPAReportSelfCheckStatusRsp(VOID *pvMsg, UINT32 ulMsgLen)
{
    GosLog(LOG_DETAIL, "recv PIS_SELFCHECK_STATUS msg");

    g_bNeedSendSelfCheckMsg = FALSE;
}

/**
 * @brief           解析DC预录音广播请求的应答包
 * @param           pvMsg       [in]
 * @param           ulMsgLen    [in]
 * @return          VOID 
 * @author          zhengwenjie(zhengwenjie@gbcom.com.cn)
 * @par 示例:
 * @code
 *  
 * @endcode
 * @note
 * 将预录音广播的结果返回给DC
 */
static VOID OnPARecBrdRsp(VOID *pvMsg, UINT32 ulMsgLen)
{
    PA_REC_BRD_RSP_T    *pstRsp = (PA_REC_BRD_RSP_T*)pvMsg;

    GosLog(LOG_DETAIL, "recv PA_REC_BRD_RSP msg");
    if (ulMsgLen != sizeof(PA_REC_BRD_RSP_T))
    {
        GosLog(LOG_ERROR, "invalid msg len(%u) of PA_REC_BRD_RSP", ulMsgLen);
        return;
    }

    // 发送消息给DIS
    SendSAStartRecBrdRspMsg(pstRsp);
}

static VOID OnPARecCfgRsp(VOID *pvMsg, UINT32 ulMsgLen)
{
    UINT8       *pucTmp = (UINT8*)pvMsg;
    UINT16      usRecNum = 0;
    UINT16      *pusRecID;
    UINT16      *pusLen;
    CHAR        *szRecInfo;
    CHAR        acRecInfo[1024];
    UINT32      ulLen = 0;

    if (ulMsgLen < sizeof(UINT16))
    {
        GosLog(LOG_ERROR, "invalid msg len %u of PA_REC_CFG_RSP", ulMsgLen);
        return;
    }

    for (UINT32 i=0; i<g_ulRecBrdCfgNum; i++)
    {
        GOS_FREE(g_astRecBrdCfg[g_ulRecBrdCfgNum].szValue);
    }

    g_ulRecBrdCfgNum = 0;

    GosLog(LOG_DETAIL, "recv PA_REC_CFG_RSP msg");
    usRecNum = GET_SHORT(pucTmp);
    usRecNum = ntohs(usRecNum);

    ulLen += sizeof(UINT16);
    while(ulLen < ulMsgLen)
    {
        if ((ulMsgLen - ulLen) <= 4)
        {
            GosLog(LOG_ERROR, "msg error");
            break;
        }

        pusRecID = (UINT16*)(pucTmp+ulLen);
        *pusRecID = ntohs(*pusRecID);
        ulLen += sizeof(UINT16);

        pusLen = (UINT16*)(pucTmp+ulLen);
        *pusLen = ntohs(*pusLen);
        ulLen += sizeof(UINT16);

        szRecInfo = (CHAR*)(pucTmp+ulLen);
        ulLen += *pusLen;

        memset(acRecInfo, 0, sizeof(acRecInfo));
        strncpy(acRecInfo, szRecInfo, sizeof(acRecInfo)-1);
        if (*pusLen < sizeof(acRecInfo)-1)
        {
            acRecInfo[*pusLen] = '\0';
        }

        GosLog(LOG_INFO, "rec brd info %u: %s", *pusRecID, szRecInfo);

        if (g_ulRecBrdCfgNum < ARRAY_SIZE(g_astRecBrdCfg))
        {
            g_astRecBrdCfg[g_ulRecBrdCfgNum].ulID = *pusRecID;
            g_astRecBrdCfg[g_ulRecBrdCfgNum].szValue = strdup(szRecInfo);
            g_ulRecBrdCfgNum++;
        }
    }

    g_ulTotalCheckPISCount = 0xffffffff;
}

static VOID OnPAHeartBeat(VOID *pvMsg, UINT32 ulMsgLen)
{
    PA_GET_SYS_STATUS_RSP_T    *pstRsp = (PA_GET_SYS_STATUS_RSP_T*)pvMsg;

    if (ulMsgLen != sizeof(PA_GET_SYS_STATUS_RSP_T))
    {
        GosLog(LOG_ERROR, "invalid msg len(%u) of PA heart_beat msgbody", ulMsgLen);
        return ;
    }

    // 发送消息给DIS
    SendSAPAHeartBeatRspMsg(pstRsp);

    GosLog(LOG_DETAIL, "recv PA heartbeat msg!");
}

VOID OnPAMsg(PA_MSG_HDR_T *pstMsg)
{
    VOID *pvMsgBody = (VOID*)(pstMsg+1);
    UINT32 ulMsgType = ntohs(pstMsg->usMsgType);
    UINT32 ulMsgLen  = ntohs(pstMsg->usMsgLen);

    if (ulMsgType == EV_PA_START_REAL_BRD)
    {
        OnPAStartBrdRsp(pvMsgBody, ulMsgLen);
    }
    else if (ulMsgType == EV_PA_STOP_REAL_BRD)
    {
        OnPAEndBrdRsp(pvMsgBody, ulMsgLen);
    }
    else if (ulMsgType == EV_PIS_GET_TRAIN_INFO)
    {
        OnPAGetTrainInfoRsp(pvMsgBody, ulMsgLen);
    }
    else if (ulMsgType == EV_PIS_SELFCHECK_STATUS)
    {
        OnPAReportSelfCheckStatusRsp(pvMsgBody, ulMsgLen);
    }
    else if (ulMsgType == EV_PA_REC_BRD)
    {
        OnPARecBrdRsp(pvMsgBody, ulMsgLen);
    }
    else if (ulMsgType == EV_PA_REC_CFG)
    {
        OnPARecCfgRsp(pvMsgBody, ulMsgLen);
    }
    else if (ulMsgType == EV_PA_GET_SYS_STATUS)
    {
        OnPAHeartBeat(pvMsgBody, ulMsgLen);
    }
    else
    {
        GosLog(LOG_ERROR, "invalid pa msg type %04X", ulMsgType);
    }
}
