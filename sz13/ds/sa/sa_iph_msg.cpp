#include "sa_public.h"
/**
 * @note
 * 本接口适用于接口文档\\10.1.1.154\00-网络u盘\地铁集群二开项目\深圳14\文档资料\《深圳地铁14号线LTE系统与车辆接口文件V1.2》
 * 特别注意该接口与上海18号线有较大区别
 */

BOOL        g_bNeedSendSelfCheckMsg = TRUE;
UINT32      g_ulTotalCheckPISCount = 0;
UINT32      g_ulRecBrdCfgNum = 0;
ID_VALUE_T  g_astRecBrdCfg[1024];

extern BOOL GetSelfCheckStatus(INT32 *piAppSelfCheckStatus);

extern UINT32  g_ulIPHRunStatus;
extern UINT32  g_ulIPHLinkcheckMaxNum;

// IPH请求时，需要将请求中的rtsp地址和端口解析处理，url中如果没有端口，缺省就是554；解析后将车厢号+IPH设备编号 与rtsp服务器地址、端口建立一个map映射（主键可用车厢号<<16 | IPHDevID生成一个UINT32的整数）；另外，需要将发送给DIS的请求中的VideoURL替换成本地地址和端口，URL只呢个其它字符内容不变
std::map<UINT32, RTSP_INFO_T>     g_mRTSPInfo;

VOID AddRTSPInfo(UINT8 ucCarriageID, UINT8 ucIPHDevID, RTSP_INFO_T &stRtspInfo)
{
    UINT32      ulKey = (((UINT32)ucCarriageID) << 16) | ucIPHDevID;

    GosLog(LOG_INFO, "add RTSP info: %u %u -> rtsp server "IP_FMT ":%u, local rtsp server" IP_FMT ":%u", 
            ucCarriageID, ucIPHDevID,
            IP_ARG(stRtspInfo.aucRTSPServerAddr), stRtspInfo.usRTSPServerPort,
            IP_ARG(stRtspInfo.aucLocalRTSPServerAddr), stRtspInfo.usLocalRTSPServerPort);
  
    g_mRTSPInfo[ulKey] = stRtspInfo;
}

BOOL GetRTSPInfo(UINT8 ucCarriageID, UINT8 ucIPHDevID, RTSP_INFO_T &stRtspInfo)
{
    UINT32      ulKey = (((UINT32)ucCarriageID) << 16) | ucIPHDevID;

    if (g_mRTSPInfo.find(ulKey) == g_mRTSPInfo.end())
    {
        return FALSE;
    }

    memcpy(&stRtspInfo, &g_mRTSPInfo[ulKey], sizeof(stRtspInfo));

    return TRUE;
}

/*
BOOL IsSameRTPPort(UINT16 usPort1, UINT16 usPort2)
{
    if (usPort1 == 0)
    {
        usPort1 = DEFAULT_RTP_PORT;
    }

    if (usPort2 == 0)
    {
        usPort2 = DEFAULT_RTP_PORT;
    }

    return usPort1 == usPort2;
}

BOOL ConvertRtspServerAddr(UINT8 *pucLocalRTSPServerAddr, UINT16 usLocalRTSPServerPort, UINT8 *pucRTSPServerAddr, UINT16 &usRTSPServerPort)
{
    std::map<UINT32, IP_ADDR_T>::iterator it = g_mRTSPInfo.begin();

    for (; it!=g_mRTSPInfo.end(); it++)
    {
        if (memcpy(it->second.aucIP, pucLocalRTSPServerAddr, 4) == 0 &&
            IsSameRTPPort(it->second.usPort, usLocalRTSPServerPort) )
        {
            //memcpy(pucRTSPServerAddr, it->first.
        }
    }

    //TODO
    SET_IP(pucRTSPServerAddr, 192, 0, 1, 133);
    usRTSPServerPort = DEFAULT_RTP_PORT;

    return TRUE;
}
*/

BOOL SendPISMsg(UINT16 usMsgType, VOID *pvMsg, UINT16 usMsgLen)
{
    if (g_stPISTCPClientSock == INVALID_SOCKET)
    {
        GosLog(LOG_ERROR, "send PIS msg failed for invalid socket");
        return FALSE;
    }

    PIS_MSG_HDR_T   *pstMsg = (PIS_MSG_HDR_T*)calloc(1, sizeof(PIS_MSG_HDR_T)+usMsgLen);

    if (!pstMsg)
    {
        GosLog(LOG_ERROR, "malloc failed when sending PIS msg");
        return FALSE;
    }

    pstMsg->usMsgMagic = htons(PIS_MSG_MAGIC);
    pstMsg->usMsgType = htons(usMsgType);
    pstMsg->usMsgLen = htons(usMsgLen);

    if (pvMsg && usMsgLen)
    {
        memcpy(pstMsg+1, pvMsg, usMsgLen);
    }

    if (!gos_tcp_send(g_stPISTCPClientSock, pstMsg, sizeof(PIS_MSG_HDR_T) + usMsgLen))
    {
        GosLog(LOG_ERROR, "send PIS msg(%u) failed", usMsgType);
        InitPISSocket();
        free(pstMsg);

        return FALSE;
    }

    free(pstMsg);
    //g_ulLastPISMsgTime = gos_get_uptime();

    return TRUE;
}

VOID SendPISHeartBeatMsg()
{
    UINT32          ulCurrTime = gos_get_uptime_1ms();
    static UINT32   ulLastPISMsgTime = 0;

    if (ulLastPISMsgTime == 0 ||
        ulCurrTime >= (ulLastPISMsgTime+g_stSACfg.ulPISHeartBeatTime*1000))
    {
        SendPISMsg(EV_PIS_HEART_BEAT);
        ulLastPISMsgTime = ulCurrTime;

        GosLog(LOG_DETAIL, "send pis heartbeat end");
    }
}

VOID SendPISSelfCheckMsg()
{
    static UINT32   ulSendTime = 0;
    INT32           iAppSelfCheckStatus;
    UINT32          ulTime = gos_get_uptime() - ulSendTime;
    UINT32          ulSendPeriod = 3;
    
    if (ulTime >= ulSendPeriod)
    {
        PIS_SELFCHECK_STATUS_REQ_T  stMsg = {0};

        if (!GetSelfCheckStatus(&iAppSelfCheckStatus))
        {
            return;
        }

        if (iAppSelfCheckStatus == APP_SELF_CHECK_STATUS_UNKNOWN)
        {
            GosLog(LOG_DETAIL, "app self check status is unknown");
            return ;
        }

        stMsg.ucCheckStatus = iAppSelfCheckStatus;
        SendPISMsg(EV_PIS_SELFCHECK_STATUS, &stMsg, sizeof(stMsg));

        ulSendTime = gos_get_uptime();
        GosLog(LOG_DETAIL, "send  app self check status to PIS end, app self check status is %u", stMsg.ucCheckStatus);
    }

    return;
}

VOID SendPISGetTrainInfoMsg()
{
    static UINT32   ulLastSendTime = 0;
    UINT32      ulCurrTime = gos_get_uptime();
    GJsonParam  Param;

    if (ulLastSendTime > 0 && ulCurrTime < (5+ulLastSendTime))
    {
        return;
    }

    GosLog(LOG_DETAIL, "send pis get train info msg");
    SendPISMsg(EV_PIS_GET_TRAIN_INFO);

    ulLastSendTime = gos_get_uptime();
    if (ulLastSendTime == 0)
    {
        ulLastSendTime = 1;
    }
}

VOID SendPISRecBrdCfgMsg()
{
    SendPISMsg(EV_PA_REC_CFG);
}

/**
 * @brief           向IPH服务器发送获取获取预录音广播配置请求
 * @return          VOID 
 * @author          zhengwenjie(zhengwenjie@gbcom.com.cn)
 * @par 示例:
 * @code
 *  
 * @endcode
 * @note
 * 根据文档选择
 */
VOID CheckPISRecBrdCfg()
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
        SendPISRecBrdCfgMsg();
        GosLog(LOG_INFO, "send get pis rec brd cfg msg");

        ulLastSendTime = gos_get_uptime();
        if (ulLastSendTime == 0)
        {
            ulLastSendTime = 1;
        }

        g_ulTotalCheckPISCount++;
    }
}

/**
 * @brief           解析PIS发送的列车信息应答
 * @param           pvMsg       [in] 收到的应答
 * @param           ulMsgLen    [in] 消息体长度
 * @return          VOID 
 * @author          zhengwenjie(zhengwenjie@gbcom.com.cn)
 * @par 示例:
 * @code
 *  
 * @endcode
 * @note
 * 将列车的驾驶模式和盖状态设置到UCI
 * 盖状态：1：打开 0：关闭
 * 驾驶模式：1：有人驾驶 0：无人驾驶
 */
/*
static VOID OnPISGetTrainInfoRsp(VOID *pvMsg, UINT32 ulMsgLen)
{
    static UINT8                ucCoverStatus = 0xff;
    static UINT8                ucDriverMode = 0xff;
    PIS_GET_TRAIN_INFO_RSP_T    *pstRsp = (PIS_GET_TRAIN_INFO_RSP_T*)pvMsg;

    GosLog(LOG_DETAIL, "recv PIS_GET_TRAIN_INFO msg");
    if (ulMsgLen != sizeof(PIS_GET_TRAIN_INFO_RSP_T))
    {
        GosLog(LOG_ERROR, "invalid msg len(%u) of PIS_GET_TRAIN_INFO", ulMsgLen);
        return;
    }

    // 存储设备状态
    if (pstRsp->ucCoverStatus != ucCoverStatus ||
        pstRsp->ucDriverMode  != ucDriverMode )
    {
        ucCoverStatus = pstRsp->ucCoverStatus;
        ucDriverMode  = pstRsp->ucDriverMode;

        GosLog(LOG_INFO, "cover status is %u, driver mode is %u", ucCoverStatus, ucDriverMode);

        CfgSet(TRAIN_COVER_STATUS, ucCoverStatus);
        CfgSet(TRAIN_DRIVE_MODE, ucDriverMode);
//        CfgCommit();
    }
}
*/ 
/*
static VOID OnPISReportSelfCheckStatusRsp(VOID *pvMsg, UINT32 ulMsgLen)
{
    GosLog(LOG_DETAIL, "recv PIS_SELFCHECK_STATUS msg");

    g_bNeedSendSelfCheckMsg = FALSE;
}
*/

/**
 * @brief           解析IPH服务器设备状态信息应答
 * @param           pvMsg       [in] 收到的应答消息
 * @param           ulMsgLen    [in] 消息体长度
 * @return          VOID 
 * @author          zhengwenjie(zhengwenjie@gbcom.com.cn)
 * @par 示例:
 * @code
 *  
 * @endcode
 * @note
 * 解析消息后将IPH状态组织成JSON发送给DIS
 * IPH系统状态：0：正常　其他值：异常
 */
static VOID OnPISHeartBeat(VOID *pvMsg, UINT32 ulMsgLen)
{
    IPH_GET_SYS_STATUS_RSP_T    *pstRsp = (IPH_GET_SYS_STATUS_RSP_T*)pvMsg;

    if (ulMsgLen != sizeof(IPH_GET_SYS_STATUS_RSP_T))
    {
        GosLog(LOG_ERROR, "invalid msg len(%u) of IPH heart_beat msgbody", ulMsgLen);
        return ;
    }

    g_ulIPHRunStatus = pstRsp->ucStatus;

    ram_set(TRAIN_IPH_RUNNING_STATUS, g_ulIPHRunStatus);

    g_ulIPHLinkcheckMaxNum = 0;

    GosLog(LOG_DETAIL, "recv IPH heartbeat msg!");
}

// rtsp://admin;admin@10.194.128.68:2203/h264/ch1/av_stream
BOOL GetRtspAddrFromUrl(CHAR *szUrl, UINT8 *pucAddr, UINT16 *pusPort)
{
    CHAR        acVideoURL[32];
    CHAR        *szStart;
    CHAR        *szEnd;
    CHAR        *szTmp;
    UINT32      ulUrlLen;
    
    szStart = strstr(szUrl, "rtsp://");
    if (!szStart)
    {
        return FALSE;
    }

    szStart += strlen("rtsp://");
    szEnd = strchr(szStart, '/');
    if (!szEnd)
    {
        return FALSE;
    }

    szTmp = strchr(szStart, '@');
    if (szTmp)
    {
        szStart = szTmp+1;
    }

    ulUrlLen = szEnd - szStart;
    if (ulUrlLen >= sizeof(acVideoURL))
    {
        return FALSE;
    }

    memset(acVideoURL, 0, sizeof(acVideoURL));
    memcpy(acVideoURL, szStart, ulUrlLen);

    return gos_get_ip_port(acVideoURL, pucAddr, pusPort);
}

// 将原始的url替换成车载台本地地址和端口的url (rtsp://192.168.90.223:55554/h264/ch1/sub/av_stream)
BOOL ReplaceRtspVideoUrl(CHAR *szOriVideoUrl, UINT8 *pucLocalRTSPServerAddr, UINT16 usLocalRTSPServerPort, CHAR *szVideoUrl, UINT32 ulMaxLen)
{
    CHAR    *szTmp = strstr(szOriVideoUrl, "rtsp://");

    if (!szTmp)
    {
        return FALSE;
    }

    szTmp += strlen("rtsp://");
    szTmp = strchr(szTmp, '/');
    if (!szTmp)
    {
        return FALSE;
    }

    UINT32 ulLen;

    if (usLocalRTSPServerPort == DEFAULT_RTP_PORT ||
        usLocalRTSPServerPort == 0)
    {
        ulLen = sprintf(szVideoUrl, "rtsp://" IP_FMT, IP_ARG(pucLocalRTSPServerAddr));
    }
    else
    {
        ulLen = sprintf(szVideoUrl, "rtsp://" IP_FMT":%u", IP_ARG(pucLocalRTSPServerAddr), usLocalRTSPServerPort);
    }

    if ((ulLen+strlen(szTmp)) >= ulMaxLen)
    {
        return FALSE;
    }

    strcpy(szVideoUrl+ulLen, szTmp);
    return TRUE;
}

/**
 * @brief           处理IPH对讲请求
 * @param           pvMsg      [in]
 * @param           ulMsgLen   [in]
 * @return          VOID 
 * @author          zhengwenjie(zhengwenjie@gbcom.com.cn)
 * @par 示例:
 * @code
 *  
 * @endcode
 * @note
 */
static VOID OnIPHTalkApplyReq(VOID *pvMsg, UINT32 ulMsgLen)
{
    IPH_TALK_APPLY_REQ_T    *pstReq = (IPH_TALK_APPLY_REQ_T*)pvMsg;

    GosLog(LOG_INFO, "recv IPH_TALK_APPLY_REQ msg");
    if (ulMsgLen < STRUCT_OFFSET(IPH_TALK_APPLY_REQ_T, astApplyReq))
    {
        GosLog(LOG_ERROR, "invalid msg len(%u) of IPH_TALK_APPLY_REQ", ulMsgLen);
        return;
    }

    if (ulMsgLen != (STRUCT_OFFSET(IPH_TALK_APPLY_REQ_T, astApplyReq) + pstReq->ucNum*sizeof(pstReq->astApplyReq[0])))
    {
        GosLog(LOG_ERROR, "invalid msg len(%u) of IPH_TALK_APPLY_REQ", ulMsgLen);
        return;
    }

    if (0 == pstReq->ucNum)
    {
        GosLog(LOG_ERROR, "IPH req number is 0");
        return;
    }

    if (!g_stSACfg.bIPHVideoProxyMode)
    {
        // 发送消息给DIS
        SendSAApplyIPHTalkReqMsg(pstReq);
        return;
    }

    RTSP_INFO_T     stRTSPInfo;
    CHAR            acVideoUrl[96];
    GJsonTupleParam Param;

    for (UINT32 i=0; i<pstReq->ucNum; i++)
    {
        GosLog(LOG_INFO, "get IPH talk request %u: %u %u %u %s", i+1,
            ntohs(pstReq->astApplyReq[i].usTrainUnitID),
            pstReq->astApplyReq[i].ucCarriageID,
            pstReq->astApplyReq[i].ucIPHDevID,
            pstReq->astApplyReq[i].acVideoUrl);

        memset(&stRTSPInfo, 0, sizeof(stRTSPInfo));
        stRTSPInfo.stPreAllocedSocket = INVALID_SOCKET;

        if (!GetRtspAddrFromUrl(pstReq->astApplyReq[i].acVideoUrl, stRTSPInfo.aucRTSPServerAddr, &stRTSPInfo.usRTSPServerPort))
        {
            GosLog(LOG_ERROR, "parse IPH video url failed: %s", pstReq->astApplyReq[i].acVideoUrl);
            continue;
        }

        if (!AllocLocalRTSPServerAddr(stRTSPInfo.aucLocalRTSPServerAddr, stRTSPInfo.usLocalRTSPServerPort, stRTSPInfo.stPreAllocedSocket))
        {
            GosLog(LOG_ERROR, "alloc local rtsp addr failed");
            continue;
        }

        stRTSPInfo.ulApplyUptime = gos_get_uptime();
        stRTSPInfo.ulRTSPStatus = RTSP_STATUS_PRE_ALLOC;

        if (!ReplaceRtspVideoUrl(pstReq->astApplyReq[i].acVideoUrl, stRTSPInfo.aucLocalRTSPServerAddr, 
                                 stRTSPInfo.usLocalRTSPServerPort, acVideoUrl, sizeof(acVideoUrl)-1))
        {
            GosLog(LOG_ERROR, "replace iph url of failed");
            continue;
        }
        
        AddRTSPInfo(pstReq->astApplyReq[i].ucCarriageID, pstReq->astApplyReq[i].ucIPHDevID, stRTSPInfo);

        GJsonParam      SubParam;

        pstReq->astApplyReq[i].usTrainUnitID = ntohs(pstReq->astApplyReq[i].usTrainUnitID);

        SubParam.Add("TrainUnitID", pstReq->astApplyReq[i].usTrainUnitID);
        SubParam.Add("SAID", g_acSAID);
        SubParam.Add("CarriageID", pstReq->astApplyReq[i].ucCarriageID);
        SubParam.Add("IPHDevID", pstReq->astApplyReq[i].ucIPHDevID);
        SubParam.AddIP("IPHDevIP", g_stSACfg.stPISAddr.aucIP);
        SubParam.Add("VideoURL", acVideoUrl);
        
        Param.Add(SubParam);
    }

    SendDISMsg(MSG_SAApplyIPHTalk, 0, Param);
}

static VOID OnIPHTalkStopReq(VOID *pvMsg, UINT32 ulMsgLen)
{
    IPH_TALK_STOP_REQ_T *pstReq = (IPH_TALK_STOP_REQ_T*)pvMsg;

    GosLog(LOG_DETAIL, "recv IPH_TALK_STOP_REQ msg");
    if (ulMsgLen != sizeof(IPH_TALK_STOP_REQ_T))
    {
        GosLog(LOG_ERROR, "invalid msg len(%u) of IPH_TALK_STOP_REQ", ulMsgLen);
        return;
    }

    NTOHS_IT(pstReq->usTrainUnitID);

    if (g_stSACfg.bIPHVideoProxyMode)
    {
        StopRTSP(pstReq->ucCarriageID, pstReq->ucIPHDevID);
    }

    // 发送消息给DIS
    SendSAStopIPHTalkReqMsg(pstReq);
}

VOID OnPISMsg(PIS_MSG_HDR_T *pstMsg)
{
    VOID    *pvMsgBody = (VOID*)(pstMsg+1);
    UINT32  ulMsgType = ntohs(pstMsg->usMsgType);
    UINT32  ulMsgLen = ntohs(pstMsg->usMsgLen);

//    g_ulLastPISMsgTime = gos_get_uptime_1ms();

    if (ulMsgType == EV_PIS_HEART_BEAT)
    {
        OnPISHeartBeat(pvMsgBody, ulMsgLen);
    }
    else if (ulMsgType == EV_IPH_TALK_APPLY)
    {
        OnIPHTalkApplyReq(pvMsgBody, ulMsgLen);
    }
    else if (ulMsgType == EV_IPH_STOP_TALK)
    {
        OnIPHTalkStopReq(pvMsgBody, ulMsgLen);
    }
    else
    {
        GosLog(LOG_ERROR, "invalid PIS msg type %04X", ulMsgType);
    }
}